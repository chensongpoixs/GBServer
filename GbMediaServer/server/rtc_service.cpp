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
#include "server/gb_media_service.h"
 
#include "rtc_base/string_encode.h"
 
#include "libmedia_transfer_protocol/librtc/stun.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "libmedia_transfer_protocol/libhttp/http_context.h"
#include "libmedia_transfer_protocol/libnetwork/tcp_session.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h"
#include "json/json.h"
namespace  gb_media_server
{
	RtcService::RtcService()
		:  task_queue_factory_(webrtc::CreateDefaultTaskQueueFactory())
	{
		// init rtc
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Init();
		libmedia_transfer_protocol::libsrtp::SrtpSession::InitSrtpLibrary();
	}
	RtcService::~RtcService()
	{
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Destroy();
		libmedia_transfer_protocol::libsrtp::SrtpSession::DestroySrtpLibrary();
	}
#if 0
	bool RtcService::StartWebServer(const char * ip, uint16_t port)
	 {
		// http_server_  = (std::make_unique<libmedia_transfer_protocol::libhttp::HttpServer>());
		
		 
		// return false;
	 }
	 void RtcService::OnRequest(libmedia_transfer_protocol::libhttp::TcpSession * conn, 
		 const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
		 const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet)
	 {
		
	 }
#endif //
#if 0
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
#endif 

	 void RtcService::AddConsumer(std::shared_ptr<RtcConsumer> consumer)
	 {
		 std::lock_guard<std::mutex> lk(lock_);
		 name_consumers_.emplace(consumer->LocalUFrag(), consumer);
	 }

	 void RtcService::RemoveConsumer(std::shared_ptr<RtcConsumer> consumer)
	 {
		 std::lock_guard<std::mutex> lk(lock_);
		 name_consumers_.erase(consumer->LocalUFrag());
		  
		 std::string key = consumer->RemoteAddress().ipaddr().ToString() + ":" + std::to_string(consumer->RemoteAddress().port());
		 consumers_.erase(key);
	 }

	 webrtc::TaskQueueFactory * RtcService::GetTaskQueueFactory()
	 {
		 return task_queue_factory_.get();
	 }

	 void RtcService::OnStun(rtc::AsyncPacketSocket * socket, const uint8_t * data, size_t len,
		 const rtc::SocketAddress & addr, const int64_t & ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		libmedia_transfer_protocol::librtc::Stun  stun;
		if (!stun.Decode((const uint8_t *)data, len))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << " stun parse failed !!!" << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
			return;
		}
		std::shared_ptr< RtcConsumer>  consumer;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_consumers_.find(stun.LocalUFrag());
		if (iter != name_consumers_.end())
		{
			consumer = iter->second;
			stun.SetPassword(consumer->LocalPasswd()); 
			consumer->SetRemoteAddress(addr);
			stun.SetMessageType(libmedia_transfer_protocol::librtc::kStunMsgBindingResponse);
			uint32_t  mapped_addr = 0;

			//mapped_addr = libmedia_transfer_protocol::ByteReader<uint32_t>::ReadBigEndian(&(const uint8_t *)(&(addr.ipaddr().ipv4_address().S_un.S_addr)));
			stun.SetMappedAddr(ntohl(addr.ipaddr().ipv4_address().s_addr));
			stun.SetMappedPort(addr.port());

			rtc::Buffer packet = stun.Encode();
			  
				socket->SendTo(packet.data(), packet.size(), addr, rtc::PacketOptions());
			 
		}
		else
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "not find  UFrag: "<< stun.LocalUFrag();
			//return;
		}


		if (consumer)
		{ 
			std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
			auto iter1 = consumers_.find(key);
			if (iter1 == consumers_.end())
			{
				consumers_.emplace(key, consumer);
			} 
		}
		
	}
	void RtcService::OnDtls(rtc::AsyncPacketSocket * socket, const uint8_t * data, 
		size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		{
			std::lock_guard<std::mutex> lock(lock_);
			auto iter1 = consumers_.find(key);
			if (iter1 != consumers_.end())
			{
				iter1->second->OnDtlsRecv(data, len);
			}
			else
			{
				GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

			}
		}
	}
	void RtcService::OnRtp(rtc::AsyncPacketSocket * socket, const uint8_t * data, 
		size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		std::lock_guard<std::mutex> lock(lock_);
		auto iter1 = consumers_.find(key);
		if (iter1 != consumers_.end())
		{
			iter1->second->OnDtlsRecv(data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}
	void RtcService::OnRtcp(rtc::AsyncPacketSocket * socket, const uint8_t * data,
		size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		std::lock_guard<std::mutex> lock(lock_);
		auto iter1 = consumers_.find(key);
		if (iter1 != consumers_.end())
		{
			iter1->second->OnDtlsRecv(data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}
	 
	
}