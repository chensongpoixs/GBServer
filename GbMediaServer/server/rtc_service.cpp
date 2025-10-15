/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-14



 ******************************************************************************/

#include "server/rtc_service.h"
#include "rtc_base/logging.h"
#include "server/connection.h"
#include "server/gb_media_service.h"
#include "user/play_rtc_user.h"
#include "user/player_user.h"
#include "rtc_base/string_encode.h"
#include "utils/time_corrector.h"
#include "libmedia_transfer_protocol/librtc/stun.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
namespace  gb_media_server
{
	oatpp::Object<RtcApiDto>   RtcService::CreateOfferAnswer(const oatpp::Object<RtcApiDto>& dto)
	{
		if (GbMediaService::GetInstance().worker_thread()->IsCurrent())
		{
			std::string session_name = GetSessionNameFromUrl(dto->streamUrl);
			GBMEDIASERVER_LOG(LS_INFO) << "get session name:" << session_name;
			auto s =  GbMediaService::GetInstance().CreateSession(session_name);
			if (!s)
			{
				return RtcApiDto::createShared();
			}
			rtc::SocketAddress   server_addrs;
			server_addrs.SetIP("192.168.1.2");
			server_addrs.SetPort(10003);
			auto socket = GbMediaService::GetInstance().network_thread()->socketserver()->CreateSocket(server_addrs.ipaddr().family(), SOCK_STREAM);
			auto  connection = std::make_shared<Connection>(socket);
			auto user = s->CreatePlayerUser(connection, session_name, "", UserType::kUserTypePlayerWebRTC);
			if (!user)
			{
				GBMEDIASERVER_LOG(LS_ERROR) << "cant create user session  name:" << session_name;
				 
				return RtcApiDto::createShared();
			}

			s->AddPlayer(std::dynamic_pointer_cast<PlayerUser>(user));

			auto rtc_user = std::dynamic_pointer_cast<PlayRtcUser>(user);
			if (!rtc_user->ProcessOfferSdp(dto->sdp))
			{
				GBMEDIASERVER_LOG(LS_ERROR) << "parse sdp error. session name:" << session_name;
				return RtcApiDto::createShared();
			}

			auto answer_sdp = rtc_user->BuildAnswerSdp();
			GBMEDIASERVER_LOG(LS_INFO) << " answer sdp:" << answer_sdp;
			auto answer_ = RtcApiDto::createShared();
			answer_->sdp = answer_sdp;
			answer_->type = "answer";
			{
				std::lock_guard<std::mutex> lock(users_lock_);
				name_users_.emplace(std::make_pair(rtc_user->LocalUFrag(), rtc_user));
				GBMEDIASERVER_LOG(LS_INFO) << "insert : user: " << rtc_user->LocalUFrag();
			}
			return answer_;
		}


			 
		return  GbMediaService::GetInstance().worker_thread()->Invoke<oatpp::Object<RtcApiDto>>(RTC_FROM_HERE, [  &]() {
		
			std::string session_name = GetSessionNameFromUrl(dto->streamUrl);
			GBMEDIASERVER_LOG(LS_INFO) << "get session name:" << session_name;
			auto s = GbMediaService::GetInstance().CreateSession(session_name);
			if (!s)
			{
				return RtcApiDto::createShared();
			}
			rtc::SocketAddress   server_addrs;
			server_addrs.SetIP("192.168.1.2");
			server_addrs.SetPort(10003);
			auto socket = GbMediaService::GetInstance().network_thread()->socketserver()->CreateSocket(server_addrs.ipaddr().family(), SOCK_STREAM);
			socket->Bind(server_addrs);
			auto  connection = std::make_shared<Connection>(socket);
			auto user = s->CreatePlayerUser(connection, session_name, "", UserType::kUserTypePlayerWebRTC);
			if (!user)
			{
				GBMEDIASERVER_LOG(LS_ERROR) << "cant create user session  name:" << session_name;

				return RtcApiDto::createShared();
			}

			s->AddPlayer(std::dynamic_pointer_cast<PlayerUser>(user));

			auto rtc_user = std::dynamic_pointer_cast<PlayRtcUser>(user);
			if (!rtc_user->ProcessOfferSdp(dto->sdp))
			{
				GBMEDIASERVER_LOG(LS_ERROR) << "parse sdp error. session name:" << session_name;
				return RtcApiDto::createShared();
			}

			auto answer_sdp = rtc_user->BuildAnswerSdp();
			GBMEDIASERVER_LOG(LS_INFO) << " answer sdp:" << answer_sdp;
			auto answer_ = RtcApiDto::createShared();
			answer_->sdp = answer_sdp;
			answer_->type = "answer";
			{
				std::lock_guard<std::mutex> lock(users_lock_);
				name_users_.emplace(std::make_pair(rtc_user->LocalUFrag(), rtc_user));
				GBMEDIASERVER_LOG(LS_INFO) << "insert : user: " << rtc_user->LocalUFrag();
			}
			return answer_;
		});
	}


	void RtcService::OnStun(rtc::AsyncPacketSocket * socket, const char * data, size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		libmedia_transfer_protocol::librtc::Stun  stun;
		if (!stun.Decode((const uint8_t *)data, len))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << " stun parse failed !!!" << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
			return;
		}
		std::shared_ptr< PlayRtcUser>  rtc_user;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_users_.find(stun.LocalUFrag());
		if (iter != name_users_.end())
		{
			rtc_user = iter->second;
			stun.SetPassword(rtc_user->LocalPasswd());
			//rtc_user->SetConnection(socket);
			//rtc_user->SetSockAddr(addr);

			stun.SetMessageType(libmedia_transfer_protocol::librtc::kStunMsgBindingResponse);
			uint32_t  mapped_addr = 0;

			//mapped_addr = libmedia_transfer_protocol::ByteReader<uint32_t>::ReadBigEndian(&(const uint8_t *)(&(addr.ipaddr().ipv4_address().S_un.S_addr)));
			stun.SetMappedAddr(ntohl(addr.ipaddr().ipv4_address().s_addr));
			stun.SetMappedPort(addr.port());

			rtc::Buffer packet = stun.Encode();
			//if (packet)
			{
				//rtc::PacketOptions options;
				
				socket->SendTo(packet.data(), packet.size(), addr, rtc::PacketOptions());
				//auto naddr = webrtc_user->GetSockAddr();
				//packet->SetExt(naddr);
				//server->SendPacket(packet);
			}
		}
		else
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "not find  UFrag: "<< stun.LocalUFrag();
		}


		//if (rtc_user)
		//{
		//	auto iter1 = users_.find(addr.ipaddr().ToString());
		//	if (iter1 == users_.end())
		//	{
		//		users_.emplace(addr.ipaddr().ToString(), rtc_user);
		//	}
		//}
		
	}
	void RtcService::OnDtls(rtc::AsyncPacketSocket * socket, const char * data, size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
	}
	void RtcService::OnRtp(rtc::AsyncPacketSocket * socket, const char * data, size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
	}
	void RtcService::OnRtcp(rtc::AsyncPacketSocket * socket, const char * data, size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
	}
	 
	std::string RtcService::GetSessionNameFromUrl(const std::string &url)
	{
		//webrtc://chensong.com:9091/live/test
		//webrtc://chensong.com:9091/domain/live/test 
		std::vector<std::string> list;
		split(url, '/', &list);
		if (list.size() < 5)
		{
			return "";
		}
		std::string domain, app, stream;
		if (list.size() == 5)
		{
			domain = list[2];
			app = list[3];
			stream = list[4];
		}
		else if (list.size() == 6)
		{
			domain = list[3];
			app = list[4];
			stream = list[5];
		}

		auto pos = domain.find_first_of(':');
		if (pos != std::string::npos)
		{
			domain = domain.substr(0, pos);
		}

		return domain + "/" + app + "/" + stream;
	}
}