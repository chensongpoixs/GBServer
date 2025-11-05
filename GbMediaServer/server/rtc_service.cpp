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
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Init( "fullchain.pem", "privkey.pem" );
		libmedia_transfer_protocol::libsrtp::SrtpSession::InitSrtpLibrary();
	}
	RtcService::~RtcService()
	{
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Destroy();
		libmedia_transfer_protocol::libsrtp::SrtpSession::DestroySrtpLibrary();
	}
 


	 void RtcService::AddConsumer(std::shared_ptr<RtcInterface> rtc_interface)
	 {
		 std::lock_guard<std::mutex> lk(lock_);
		 name_rtc_interface_.emplace(rtc_interface->LocalUFrag(), rtc_interface);
	 }

	 void RtcService::RemoveConsumer(std::shared_ptr<RtcInterface> rtc_interface)
	 {
		 std::lock_guard<std::mutex> lk(lock_);
		 name_rtc_interface_.erase(rtc_interface->LocalUFrag());
 
		 std::string key = rtc_interface->RtcRemoteAddress().ipaddr().ToString() + ":" + std::to_string(rtc_interface->RtcRemoteAddress().port());
		 rtc_interfaces_.erase(key);
 
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
		std::shared_ptr< RtcInterface>  rtc_interface;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_rtc_interface_.find(stun.LocalUFrag());
		if (iter != name_rtc_interface_.end())
		{
			rtc_interface = iter->second;
			stun.SetPassword(rtc_interface->LocalPasswd());
			rtc_interface->SetRtcRemoteAddress(addr);
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


		if (rtc_interface)
		{ 
			std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
			auto iter1 = rtc_interfaces_.find(key);
			if (iter1 == rtc_interfaces_.end())
			{
				rtc_interfaces_.emplace(key, rtc_interface);
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
			auto iter1 = rtc_interfaces_.find(key);
			if (iter1 != rtc_interfaces_.end())
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
		auto iter1 = rtc_interfaces_.find(key);
		if (iter1 != rtc_interfaces_.end())
		{
			iter1->second->OnSrtpRtp((uint8_t *)data, len);
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
		auto iter1 = rtc_interfaces_.find(key);
		if (iter1 != rtc_interfaces_.end())
		{
			iter1->second->OnSrtpRtcp((uint8_t *)data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}
	 
	
}