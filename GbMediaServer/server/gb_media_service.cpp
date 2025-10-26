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
//extern "C"
//{

#include "rtc_base/string_encode.h"

//}
#include "server/session.h"
#include "utils/string_utils.h" 

#include "libmedia_transfer_protocol/libnetwork/connection.h"
 

namespace  gb_media_server
{
	namespace
	{
		static std::shared_ptr < Session> session_null;
	}
	GbMediaService::GbMediaService()
		: context_  ( libp2p_peerconnection::ConnectionContext::Create())
		, rtc_server_(new libmedia_transfer_protocol::librtc::RtcServer())
	{
		rtc_server_->SignalStunPacket.connect(&RtcService::GetInstance(), &RtcService::OnStun);
		rtc_server_->SignalDtlsPacket.connect(&RtcService::GetInstance(), &RtcService::OnDtls);
		rtc_server_->SignalRtpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtp);
		rtc_server_->SignalRtcpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtcp);
	}
	GbMediaService::~GbMediaService()
	{
	}
	std::shared_ptr < Session> GbMediaService::CreateSession(const std::string &session_name, bool split  )
	{
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = sessions_.find(session_name);
		if (iter != sessions_.end())
		{
			return iter->second;
		}
		if (split)
		{
			std::vector<std::string> list;
			string_utils::split(session_name, '/', &list);

			if (list.size() < 2 )
			{
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << "create session failed. Invalid session name:" << session_name;
				return session_null;
			}

		}
		 
		
		auto s = std::make_shared<Session>(session_name);
		//s->SetAppInfo(app_info);
		//sessions_[session_name] = s;
		sessions_.emplace(session_name, s);
		GBMEDIASERVER_LOG(LS_INFO) << "create session success. session_name:" << session_name << " now:" << rtc::TimeMillis();
		return s;
	}
	std::shared_ptr < Session> GbMediaService::FindSession(const std::string &session_name  )
	{
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = sessions_.find(session_name);
		if (iter != sessions_.end())
		{
			return iter->second;
		}
		return session_null;
	}
	bool GbMediaService::CloseSession(const std::string &session_name  )
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
	libmedia_transfer_protocol::libnetwork::TcpServer * GbMediaService::OpenTcpServer(const std::string & stream_id, uint16_t port)
	{
		// workthread
		RTC_DCHECK_RUN_ON(worker_thread());
		auto iter =  rtp_server_.find(stream_id);
		if (iter != rtp_server_.end())
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "open tcp rtp Server failed !!!  rtp map find  stsream_id:" << stream_id;
			return nullptr;
		}
		//if (worker_thread()->IsCurrent())
		auto rtp_server = std::make_unique< libmedia_transfer_protocol::libnetwork::TcpServer>();
		if (!rtp_server)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "open tcp rtp Server failed !!!  create socket failed   stsream_id:" << stream_id;
			return nullptr;
		}
		rtp_server->SignalOnNewConnection.connect(this, &GbMediaService::OnNewConnection);
		rtp_server->SignalOnRecv.connect(this, &GbMediaService::OnRecv);
		rtp_server->SignalOnSent.connect(this, &GbMediaService::OnSent);
		rtp_server->SignalOnDestory.connect(this, &GbMediaService::OnDestory);
		bool ret =  rtp_server->network_thread()->Invoke<bool>(RTC_FROM_HERE, [&]() {

			return rtp_server->Startup("0.0.0.0", port);
		});
		//auto pi =
			rtp_server_.insert(std::make_pair(stream_id, std::move(rtp_server)));
			 iter = rtp_server_.find(stream_id);
			if (iter != rtp_server_.end())
			{
				return iter->second.get();
			}

			// insert failed !!!
			return nullptr;
		 //return rtp_server_.insert(std::make_pair(stream_id, std::move(rtp_server)).second.get();
	}
	void GbMediaService::OnNewConnection(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		GBMEDIASERVER_LOG(LS_INFO);
	}
	void GbMediaService::OnDestory(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		GBMEDIASERVER_LOG(LS_INFO);
	}
	void GbMediaService::OnRecv(libmedia_transfer_protocol::libnetwork::Connection * conn, const rtc::CopyOnWriteBuffer & data)
	{
	//	GBMEDIASERVER_LOG(LS_INFO) << "";
		worker_thread()->PostTask(RTC_FROM_HERE, [=]() {
			std::shared_ptr<gb_media_server::ShareResource> user = conn->GetContext<gb_media_server::ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
			if (user)
			{
				user->OnRecv(data);
			}
		});
		
	}
	void GbMediaService::OnSent(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		GBMEDIASERVER_LOG(LS_INFO);
	}
	bool GbMediaService::Init()
	{
		
		return true;
	}
	void GbMediaService::Start(const char * ip, uint16_t port)
	{
		//libmedia_transfer_protocol::librtc::SrtpSession::InitSrtpLibrary();
		
		//network_thread_ = rtc::Thread::CreateWithSocketServer();
		//worker_thread_ = rtc::Thread::Create();
		//network_thread_->Start();
		//worker_thread_->Start();
		std::string local_ip = ip;
		rtc_server_->network_thread()->Invoke<void>(RTC_FROM_HERE, [this, local_ip, port]() {
		//worker_thread_->Invoke<void>(RTC_FROM_HERE, [this, local_ip, port]() {
			//rtc_server_ = std::make_unique<libmedia_transfer_protocol::librtc::RtcServer>(context_->network_thread());

#if 1
			
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
	void GbMediaService::Destroy()
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