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
#include "server/ws_stats_service.h"

#include "utils/file_log_writer.h"
#include "libmedia_transfer_protocol/libnetwork/local_network_interfaces.h"
#include "server/rtc_service_mgr.h"

namespace  gb_media_server
{
	namespace
	{
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 空会话对象（Null Session）
		 *  
		 *  用于在创建或查找会话失败时返回的空对象。
		 *  
		 *  @note 使用静态对象避免重复创建
		 *  @note 调用者应检查返回值是否为空
		 */
		static std::shared_ptr < Session> session_null;


		static gb_media_server::FileLogWriter g_file_logger;

		static void RtcLogCallback(const char* message)
		{
			g_file_logger.Write(message);

		}
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 构造函数（Constructor）
	 *  
	 *  初始化GB媒体服务实例，创建连接上下文、RTC服务器和Web服务。
	 *  
	 *  初始化流程：
	 *  1. 创建ConnectionContext，管理WebRTC线程和连接上下文
	 *  2. 创建RTC服务器实例
	 *  3. 创建Web服务实例
	 *  4. 连接RTC服务器的信号到RTC服务的处理方法：
	 *     - STUN数据包信号
	 *     - DTLS数据包信号
	 *     - RTP数据包信号
	 *     - RTCP数据包信号
	 *  5. 同时连接同步版本的信号（TCP版本）
	 *  
	 *  @note RTC服务器通过信号槽机制将数据包路由到RTC服务
	 *  @note 支持UDP和TCP两种传输方式
	 */
	GbMediaService::GbMediaService()
		: context_  ( libp2p_peerconnection::ConnectionContext::Create())
		//, rtc_server_(new libmedia_transfer_protocol::librtc::RtcServer())
		, rtc_servers_()
		, web_service_(new WebService)
		, rtc_service_index_(0)
	{
#if 0
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
#endif 
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 析构函数（Destructor）
	 *  
	 *  清理GB媒体服务资源。
	 *  
	 *  @note 所有资源由智能指针自动管理
	 *  @note 建议在析构前先调用Stop()和Destroy()
	 */
	GbMediaService::~GbMediaService()
	{
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 创建会话（Create Session）
	 *  
	 *  创建或获取指定名称的会话。
	 *  
	 *  创建流程：
	 *  1. 使用互斥锁保护会话映射表
	 *  2. 查找会话是否已存在，如果存在直接返回
	 *  3. 如果split为true，验证会话名称格式：
	 *     - 使用'/'分割会话名称
	 *     - 至少包含2个部分（app/stream）
	 *     - 格式不正确返回空指针
	 *  4. 创建新的Session实例
	 *  5. 将会话添加到映射表
	 *  6. 记录创建成功日志
	 *  
	 *  @param session_name 会话名称，格式为 "app/stream"
	 *  @param split 是否验证会话名称格式，默认为true
	 *  @return 返回会话的共享指针，失败返回空指针
	 *  @note 该方法线程安全
	 *  @note 如果会话已存在，返回已存在的会话
	 */
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
		 
		
		auto s = std::make_shared<Session>(session_name, rtc_service_index_++);
		//s->SetAppInfo(app_info);
		//sessions_[session_name] = s;
		sessions_.emplace(session_name, s);
		GBMEDIASERVER_LOG(LS_INFO) << "create session success. session_name:" << session_name << ", rtc_service_index:" << rtc_service_index_ << ", now:" << rtc::TimeMillis();
		return s;
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 查找会话（Find Session）
	 *  
	 *  根据会话名称查找已存在的会话。
	 *  
	 *  查找流程：
	 *  1. 使用互斥锁保护会话映射表
	 *  2. 在映射表中查找指定名称的会话
	 *  3. 如果找到返回会话指针，否则返回空指针
	 *  
	 *  @param session_name 会话名称
	 *  @return 返回会话的共享指针，不存在返回空指针
	 *  @note 该方法线程安全
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 关闭会话（Close Session）
	 *  
	 *  关闭指定的会话并释放资源。
	 *  
	 *  关闭流程：
	 *  1. 使用互斥锁保护会话映射表
	 *  2. 在映射表中查找指定名称的会话
	 *  3. 如果找到，从映射表中移除
	 *  4. 释放锁后，调用会话的Clear方法清理资源
	 *  5. 记录关闭日志
	 *  
	 *  @param session_name 会话名称
	 *  @return 如果关闭成功返回true，会话不存在返回false
	 *  @note 该方法线程安全
	 *  @note 会话的Clear方法会释放所有Producer和Consumer
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 打开TCP服务器（Open TCP Server）
	 *  
	 *  为指定的流创建TCP服务器，用于接收RTP/RTCP数据。
	 *  
	 *  创建流程：
	 *  1. 检查当前线程是否为工作线程
	 *  2. 查找流ID是否已存在TCP服务器，如果存在返回nullptr
	 *  3. 创建TcpServer实例
	 *  4. 连接TCP服务器的信号到服务的处理方法：
	 *     - 新连接信号 -> OnNewConnection
	 *     - 接收数据信号 -> OnRecv
	 *     - 发送数据信号 -> OnSent
	 *     - 连接销毁信号 -> OnDestory
	 *  5. 在网络线程中启动TCP服务器，监听指定端口
	 *  6. 将TCP服务器添加到映射表
	 *  7. 返回TCP服务器指针
	 *  
	 *  @param stream_id 流ID，用于标识流
	 *  @param port 监听端口号
	 *  @return 返回TCP服务器指针，失败返回nullptr
	 *  @note 该方法必须在工作线程中调用
	 *  @note 如果流ID已存在TCP服务器，返回nullptr
	 *  @note TCP服务器监听0.0.0.0，接受所有网卡的连接
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 新连接回调（On New Connection）
	 *  
	 *  TCP服务器接收到新连接时的回调方法。
	 *  
	 *  @param conn 新建立的连接对象指针
	 *  @note 该方法在网络线程中调用
	 *  @note 当前实现仅记录日志
	 */
	void GbMediaService::OnNewConnection(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 连接销毁回调（On Destroy Connection）
	 *  
	 *  TCP连接被销毁时的回调方法。
	 *  
	 *  @param conn 被销毁的连接对象指针
	 *  @note 该方法在网络线程中调用
	 *  @note 当前实现仅记录日志
	 *  @note 注释的代码用于清理连接上下文，已被禁用
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 接收数据回调（On Receive Data）
	 *  
	 *  TCP连接接收到数据时的回调方法。
	 *  
	 *  处理流程：
	 *  1. 将数据处理任务投递到工作线程
	 *  2. 在工作线程中从连接获取ShareResource上下文
	 *  3. 如果上下文存在，调用其OnRecv方法处理数据
	 *  
	 *  @param conn 接收数据的连接对象指针
	 *  @param data 接收到的数据缓冲区
	 *  @note 该方法在网络线程中调用
	 *  @note 实际数据处理在工作线程中异步执行
	 *  @note ShareResource可能是Producer，用于处理RTP/RTCP数据
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 发送数据回调（On Sent Data）
	 *  
	 *  TCP连接成功发送数据后的回调方法。
	 *  
	 *  @param conn 发送数据的连接对象指针
	 *  @note 该方法在网络线程中调用
	 *  @note 当前实现仅记录日志
	 */
	void GbMediaService::OnSent(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}

	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 初始化服务（Initialize Service）
	 *  
	 *  初始化GB媒体服务，加载配置文件并初始化各个组件。
	 *  
	 *  初始化流程：
	 *  1. 加载YAML配置文件
	 *  2. 如果加载失败，返回false
	 *  3. 记录配置加载成功日志
	 *  4. 初始化DTLS证书：
	 *     - 从配置文件读取公钥和私钥路径
	 *     - 初始化DtlsCerts单例
	 *  5. 初始化SRTP库
	 *  
	 *  @param config_file 配置文件路径
	 *  @return 如果初始化成功返回true，否则返回false
	 *  @note 该方法必须在Start()之前调用
	 *  @note 配置文件格式为YAML
	 *  @note DTLS证书用于WebRTC的安全连接
	 *  @note SRTP库用于RTP/RTCP的加密和解密
	 */
	bool GbMediaService::Init(const char* config_file)
	{
		rtc::LogMessage::LogThreads();
		rtc::LogMessage::LogTimestamps();
		rtc::SetRtcLogOutCallback(&RtcLogCallback);
		bool init = YamlConfig::GetInstance().LoadFile(config_file);
		//gb_media_server::YamlConfig::GetInstance().LoadFile(config_file);
		//g_file_logger.Configure(gb_media_server::YamlConfig::GetInstance().GetFileLogConfig());
		if (!init)
		{
			return init;
		}
		GBMEDIASERVER_LOG(LS_INFO) << "YamlConfig OK !!!";
		//gb_media_server::YamlConfig::GetInstance().LoadFile(config_file);
		g_file_logger.Configure(gb_media_server::YamlConfig::GetInstance().GetFileLogConfig());

		std::vector<libmedia_transfer_protocol::libnetwork::LocalNetworkInterfaceEntry>  local_ips;
		
		bool ret = libmedia_transfer_protocol::libnetwork::EnumerateLocalNetworkInterfaces(&local_ips);
		if (ret)
		{
			for (auto p : local_ips)
			{
				GBMEDIASERVER_LOG(LS_INFO) << "adapter_name=" << p.adapter_name <<", address_family =" <<p.address_family << ", ip = " << p.ip;
			}
			
		}

		 
		// init rtc
		libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Init(
			YamlConfig::GetInstance().GetRtcServerConfig().cert_public_key.c_str(),
			YamlConfig::GetInstance().GetRtcServerConfig().cert_private_key.c_str()
			//	"fullchain.pem",
		//	"privkey.pem"
		);
		libmedia_transfer_protocol::libsrtp::SrtpSession::InitSrtpLibrary();


		GBMEDIASERVER_LOG(LS_INFO) << " srtp init OK !!!";




		init = RtcServiceMgr::GetInstance().init();
		if (!init)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "rtc Service Mgr init failed !!!";
			return init;
		}

		GBMEDIASERVER_LOG(LS_INFO) << "rtc Service Mgr init OK !!!";
		return init;
		 
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 启动服务（Start Service）
	 *  
	 *  启动GB媒体服务，开始监听端口并接受连接。
	 *  
	 *  启动流程：
	 *  1. 在网络线程中启动RTC服务器：
	 *     - 监听0.0.0.0（所有网卡）
	 *     - 使用配置文件中的UDP端口
	 *  2. 启动Web服务：
	 *     - 监听0.0.0.0（所有网卡）
	 *     - 使用配置文件中的HTTP端口
	 *  
	 *  @note 该方法必须在Init()之后调用
	 *  @note RTC服务器在网络线程中启动，使用Invoke同步等待
	 *  @note Web服务在当前线程中启动
	 */
	void GbMediaService::Start(/*const char * ip, uint16_t port*/)
	{
		// start rtc server 
#if 0
		rtc_server_->network_thread()->Invoke<void>(RTC_FROM_HERE, [this]() {
			 
			
			rtc_server_->Start("0.0.0.0", 
				YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
			//	GBMEDIASERVER_LOG(LS_INFO) << "gb media start  "<< local_ip << ":"<<port<<"  OK !!!";
		});
#else 
		RtcServiceMgr::GetInstance().startup();

		GBMEDIASERVER_LOG(LS_INFO) << "rtc service Mgr Start OK !!!";
#endif // 
		web_service_->StartWebServer("0.0.0.0", 
			YamlConfig::GetInstance().GetHttpServerConfig().port);

		 
		// 启动WebSocket统计服务
		GBMEDIASERVER_LOG(LS_INFO) << "Starting WebSocket stats service...";
		if (!gb_media_server::WebSocketStatsService::GetInstance().Start()) {
			GBMEDIASERVER_LOG(LS_WARNING) << "WebSocket stats service failed to start (may be disabled or missing dependencies)";
		}
		else {
			GBMEDIASERVER_LOG(LS_INFO) << "WebSocket stats service started successfully";
		}
		
	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 停止服务（Stop Service）
	 *  
	 *  停止GB媒体服务，停止监听端口并断开所有连接。
	 *  
	 *  @note 当前实现为空，待完善
	 *  @note 建议添加停止RTC服务器和Web服务的代码
	 */
	void GbMediaService::Stop()
	{
		// 停止WebSocket统计服务
		GBMEDIASERVER_LOG(LS_INFO) << "Stopping WebSocket stats service...";
		gb_media_server::WebSocketStatsService::GetInstance().Stop();
		GBMEDIASERVER_LOG(LS_INFO) << "Stopping WebSocket stats service OK !!!";
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 销毁服务（Destroy Service）
	 *  
	 *  销毁GB媒体服务，释放所有资源。
	 *  
	 *  销毁流程：
	 *  1. 销毁DTLS证书资源
	 *  2. 销毁SRTP库资源
	 *  
	 *  @note 该方法应该在服务不再使用时调用
	 *  @note 销毁后需要重新调用Init()和Start()才能再次使用
	 */
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