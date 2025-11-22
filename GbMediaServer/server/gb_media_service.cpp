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

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。


 ******************************************************************************/
#include "server/gb_media_service.h"
 
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
#include "utils/yaml_config.h"
#include "gb_media_server_log.h"
namespace  gb_media_server
{
	namespace
	{
		static std::shared_ptr < Session> session_null;
	}
	GbMediaService::GbMediaService()
		: context_  ( libp2p_peerconnection::ConnectionContext::Create())
		, rtc_server_(new libmedia_transfer_protocol::librtc::RtcServer())
		, web_service_(new WebService)
	{
		rtc_server_->SignalStunPacket.connect(&RtcService::GetInstance(), &RtcService::OnStun);
		rtc_server_->SignalDtlsPacket.connect(&RtcService::GetInstance(), &RtcService::OnDtls);
		rtc_server_->SignalRtpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtp);
		rtc_server_->SignalRtcpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtcp);

		/// <summary>
		/// 
		/// </summary>
		rtc_server_->SignalSyncStunPacket.connect(&RtcService::GetInstance(), &RtcService::OnStun);
		rtc_server_->SignalSyncDtlsPacket.connect(&RtcService::GetInstance(), &RtcService::OnDtls);
		rtc_server_->SignalSyncRtpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtp);
		rtc_server_->SignalSyncRtcpPacket.connect(&RtcService::GetInstance(), &RtcService::OnRtcp);
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
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	void GbMediaService::OnDestory(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		//worker_thread()->PostTask(RTC_FROM_HERE, [=]() {
		//	conn->ClearContext(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
		//	//std::shared_ptr<gb_media_server::ShareResource> user = conn->GetContext<gb_media_server::ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
		//	//if (user)
		//	//{
		//	//	//user->OnRecv(data);
		//	//}
		//	});
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
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	bool GbMediaService::Init(const char* config_file)
	{
		bool init = YamlConfig::GetInstance().LoadFile(config_file);
		if (!init)
		{
			return init;
		}
		GBMEDIASERVER_LOG(LS_INFO) << "YamlConfig OK !!!";
		// init rtc
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Init(
			YamlConfig::GetInstance().GetRtcServerConfig().cert_public_key.c_str(),
			YamlConfig::GetInstance().GetRtcServerConfig().cert_private_key.c_str()
			//	"fullchain.pem",
		//	"privkey.pem"
		);
		libmedia_transfer_protocol::libsrtp::SrtpSession::InitSrtpLibrary();
		return init;
		 
	}
	void GbMediaService::Start(/*const char * ip, uint16_t port*/)
	{
		// start rtc server 
		rtc_server_->network_thread()->Invoke<void>(RTC_FROM_HERE, [this]() {
			 
			
			rtc_server_->Start("0.0.0.0", 
				YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
			//	GBMEDIASERVER_LOG(LS_INFO) << "gb media start  "<< local_ip << ":"<<port<<"  OK !!!";
		});
		 

		web_service_->StartWebServer("0.0.0.0", 
			YamlConfig::GetInstance().GetHttpServerConfig().port);

		 
		
		
	}

	
	void GbMediaService::Stop()
	{
	}
	void GbMediaService::Destroy()
	{
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Destroy();
		libmedia_transfer_protocol::libsrtp::SrtpSession::DestroySrtpLibrary();
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