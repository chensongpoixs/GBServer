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
				   date:  2025-10-13



 ******************************************************************************/
#include "server/gb_media_service.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"
#include "server/rtc_service.h"
#include "api/array_view.h"
#include "utils/time_corrector.h"
//extern "C"
//{

#include "rtc_base/string_encode.h"

//}
#include "server/session.h"

namespace  gb_media_server
{
	namespace
	{
		static std::shared_ptr < Session> session_null;
	}
	std::shared_ptr < Session> GbMediaService::CreateSession(const std::string &session_name)
	{
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = sessions_.find(session_name);
		if (iter != sessions_.end())
		{
			return iter->second;
		}
		std::vector<std::string> list;
		split(session_name, '/', &list);
		 
		if (list.size() != 3)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "create session failed. Invalid session name:" << session_name;
			return session_null;
		}
		auto s = std::make_shared<Session>(session_name);
		//s->SetAppInfo(app_info);

		sessions_.emplace(session_name, s);
		GBMEDIASERVER_LOG(LS_INFO) << "create session success. session_name:" << session_name << " now:" << rtc::TimeMillis();
		return s;
	}
	std::shared_ptr < Session> GbMediaService::FindSession(const std::string &session_name)
	{
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = sessions_.find(session_name);
		if (iter != sessions_.end())
		{
			return iter->second;
		}
		return session_null;
	}
	bool GbMediaService::CloseSession(const std::string &session_name)
	{
		std::shared_ptr < Session> s;
		{
			std::lock_guard<std::mutex> lk(lock_);
			auto iter = sessions_.find(session_name);
			if (iter != sessions_.end())
			{
				s = iter->second;
				sessions_.erase(iter);
			}
		}
		if (s)
		{
			GBMEDIASERVER_LOG(LS_INFO) << " close session:" << s->SessionName() << " now:" << rtc::TimeMillis();
			s->Clear();
		}
		return true;
	}
	void GbMediaService::Start(const char * ip, uint16_t port)
	{
		context_ = libp2p_peerconnection::ConnectionContext::Create();
		//network_thread_ = rtc::Thread::CreateWithSocketServer();
		//worker_thread_ = rtc::Thread::Create();
		//network_thread_->Start();
		//worker_thread_->Start();
		std::string local_ip = ip;
		context_->worker_thread()->Invoke<void>(RTC_FROM_HERE, [this, local_ip, port]() {
		//worker_thread_->Invoke<void>(RTC_FROM_HERE, [this, local_ip, port]() {
			rtc_server_ = std::make_unique<libmedia_transfer_protocol::librtc::RtcServer>(context_->network_thread());

#if 1
			rtc_server_->SignalStunPacket.connect(&RtcService::GetInstance(), &RtcService::OnStun);
			rtc_server_->SignalDtlsPacket.connect(&RtcService::GetInstance(), &RtcService::OnDtls);
			rtc_server_->SignalRtpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtp);
			rtc_server_->SignalRtcpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtcp);
#else 


			rtc_server_->SignalStunPacketBuffer.connect(&RtcService::GetInstance(), &RtcService::OnStun);
			rtc_server_->SignalDtlsPacketBuffer.connect(&RtcService::GetInstance(), &RtcService::OnDtls);
			rtc_server_->SignalRtpPacketBuffer.connect(&RtcService::GetInstance(), &RtcService::OnRtp);
			rtc_server_->SignalRtcpPacketBuffer.connect(&RtcService::GetInstance(), &RtcService::OnRtcp);
#endif // 
			rtc_server_->Start(local_ip.c_str(), port);
		//	GBMEDIASERVER_LOG(LS_INFO) << "gb media start  "<< local_ip << ":"<<port<<"  OK !!!";
		});
		
	}

	
	void GbMediaService::Stop()
	{
	}
	//void OnTimer(const TaskPtr &t);

	//void OnNewConnection(const TcpConnectionPtr &conn) override;
	//void OnConnectionDestroy(const TcpConnectionPtr &conn) override;
	//void OnActive(const ConnectionPtr &conn) override;
	//bool OnPlay(const TcpConnectionPtr &conn, const std::string &session_name, const std::string &param) override;
	//bool OnPublish(const TcpConnectionPtr &conn, const std::string &session_name, const std::string &param) override;
	//void OnRecv(const TcpConnectionPtr &conn, PacketPtr &&data) override;
	//void OnRecv(const TcpConnectionPtr &conn, const PacketPtr &data) override {};
	//void OnSent(const TcpConnectionPtr &conn) override;
	//bool OnSentNextChunk(const TcpConnectionPtr &conn) override;
	//void OnRequest(const TcpConnectionPtr &conn, const HttpRequestPtr &req, const PacketPtr &packet) override;

	 
}