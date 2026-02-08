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
#ifndef _C_GB_MEDIA_SERVICE_H_
#define _C_GB_MEDIA_SERVICE_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"
#include "libmedia_transfer_protocol/libnetwork/tcp_server.h"
#include "rtc_base/logging.h"
#include "server/web_service.h"
namespace  gb_media_server
{



	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief GB媒体服务类（GB Media Service）
	*  
	*  GbMediaService是GBMediaServer流媒体服务器的核心服务类，采用单例模式设计。
	*  它负责管理整个媒体服务器的生命周期，包括会话管理、网络连接管理、RTC服务器管理等。
	*  
	*  主要功能：
	*  1. 会话管理：创建、查找、关闭媒体会话（Session）
	*  2. 网络服务管理：管理TCP服务器，处理RTP/RTCP数据流
	*  3. RTC服务器管理：管理WebRTC服务器，处理WebRTC连接
	*  4. 连接管理：处理新连接、连接销毁、数据接收和发送事件
	*  5. 线程管理：提供信令线程、工作线程、网络线程的访问接口
	*  6. Web服务管理：管理HTTP API服务
	*  
	*  服务生命周期：
	*  1. 调用Init()初始化服务，加载配置文件
	*  2. 调用Start()启动服务，开始监听端口
	*  3. 服务运行期间，处理各种连接和会话请求
	*  4. 调用Stop()停止服务，停止监听
	*  5. 调用Destroy()销毁服务，释放所有资源
	*  
	*  会话管理说明：
	*  - 每个媒体流对应一个Session，Session管理Producer和Consumer
	*  - Session通过session_name唯一标识
	*  - 支持创建、查找、关闭Session操作
	*  
	*  网络连接说明：
	*  - 支持TCP服务器，用于接收RTP/RTCP数据流
	*  - 每个流可以创建独立的TCP服务器，监听不同端口
	*  - 通过信号槽机制处理连接事件（新连接、销毁、接收、发送）
	*  
	*  RTC服务器说明：
	*  - 管理WebRTC服务器，处理WebRTC推流和拉流
	*  - 通过GetRtcServer()获取RTC服务器实例
	*  
	*  @note 该类采用单例模式，通过GetInstance()获取唯一实例
	*  @note 所有操作都是线程安全的，使用互斥锁保护共享资源
	*  @note 继承自sigslot::has_slots<>，支持信号槽机制
	*  
	*  使用示例：
	*  @code
	*  // 获取服务实例
	*  auto& service = GbMediaService::GetInstance();
	*  
	*  // 初始化服务
	*  if (service.Init("config.json")) {
	*      // 启动服务
	*      service.Start();
	*      
	*      // 创建会话
	*      auto session = service.CreateSession("stream_001");
	*      
	*      // 停止服务
	*      service.Stop();
	*      service.Destroy();
	*  }
	*  @endcode
	*/
	class GbMediaService : public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 构造函数（Constructor）
		*  
	*  该构造函数用于创建GbMediaService实例。它会初始化内部成员变量，
	*  包括连接上下文、会话映射表、RTC服务器等。
	*  
	*  初始化流程：
	*  1. 创建ConnectionContext，用于管理WebRTC连接上下文
	*  2. 初始化互斥锁，保护共享资源
	*  3. 初始化会话映射表（sessions_）
	*  4. 初始化RTP服务器映射表（rtp_server_）
	*  5. 创建WebService实例，用于HTTP API服务
	*  
	*  @note 构造函数不会启动服务，需要调用Init()和Start()才能开始服务
	*  @note 该构造函数是私有的，只能通过GetInstance()获取实例
	*  
	*  使用示例：
	*  @code
	*  // 通过单例模式获取实例
	*  auto& service = GbMediaService::GetInstance();
	*  @endcode
		*/
		explicit GbMediaService()  ;
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理GbMediaService实例。它会释放所有相关资源，
		*  包括会话、RTC服务器、TCP服务器、Web服务等。
		*  
		*  清理流程：
		*  1. 停止所有TCP服务器
		*  2. 关闭所有会话
		*  3. 释放RTC服务器
		*  4. 释放Web服务
		*  5. 清理会话映射表
		*  6. 清理RTP服务器映射表
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*  @note 建议在析构前先调用Stop()和Destroy()确保资源正确释放
		*/
		virtual ~GbMediaService( );



		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取服务实例（Get Instance）
		*  
		*  该方法用于获取GbMediaService的单例实例。采用懒汉式单例模式，
		*  在第一次调用时创建实例，后续调用返回同一个实例。
		*  
		*  @return 返回GbMediaService实例的引用
		*  @note 该方法是线程安全的，C++11保证静态局部变量的初始化是线程安全的
		*  @note 返回的是引用，不需要手动释放内存
		*  
		*  使用示例：
		*  @code
		*  auto& service = GbMediaService::GetInstance();
		*  service.Init("config.json");
		*  @endcode
		*/
		static GbMediaService & GetInstance()
		{
			static GbMediaService   instance;
			//GBMEDIASERVER_LOG_F(LS_INFO) << "instance: "  << &instance;
			return instance;
		}

	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 创建会话（Create Session）
		*  
		*  该方法用于创建一个新的媒体会话。会话是管理媒体流的基本单位，
		*  每个会话对应一个媒体流，包含Producer和Consumer。
		*  
		*  @param session_name 会话名称，用于唯一标识会话，不能为空
		*  @param split 是否分离音视频流，true表示分离，false表示不分离，默认为true
		*  @return 返回创建的Session共享指针，如果创建失败返回nullptr
		*  @note 如果会话已存在，会返回已存在的会话
		*  @note 会话名称必须唯一，重复创建会返回已存在的会话
		*  @note 该方法是线程安全的
		*  
		*  使用示例：
		*  @code
		*  auto session = service.CreateSession("stream_001", true);
		*  if (session) {
		*      // 会话创建成功
		*  }
		*  @endcode
		*/
		std::shared_ptr < Session> CreateSession(const std::string &session_name, bool split=true);
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 查找会话（Find Session）
		*  
		*  该方法用于根据会话名称查找已存在的会话。
		*  
		*  @param session_name 会话名称，用于查找会话
		*  @return 返回找到的Session共享指针，如果不存在返回nullptr
		*  @note 该方法是线程安全的
		*  
		*  使用示例：
		*  @code
		*  auto session = service.FindSession("stream_001");
		*  if (session) {
		*      // 会话存在
		*  }
		*  @endcode
		*/
		std::shared_ptr < Session> FindSession(const std::string &session_name );
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 关闭会话（Close Session）
		*  
		*  该方法用于关闭指定的会话，释放会话占用的所有资源。
		*  
		*  @param session_name 会话名称，用于标识要关闭的会话
		*  @return 如果关闭成功返回true，如果会话不存在返回false
		*  @note 关闭会话会释放所有相关的Producer和Consumer
		*  @note 该方法是线程安全的
		*  
		*  使用示例：
		*  @code
		*  if (service.CloseSession("stream_001")) {
		*      // 会话关闭成功
		*  }
		*  @endcode
		*/
		bool CloseSession(const std::string &session_name);
		//void OnTimer(const TaskPtr &t);


		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 打开TCP服务器（Open TCP Server）
		*  
		*  该方法用于为指定的流创建一个TCP服务器，用于接收RTP/RTCP数据流。
		*  每个流可以创建独立的TCP服务器，监听不同的端口。
		*  
		*  @param stream_id 流ID，用于标识流，不能为空
		*  @param port 监听端口号，必须大于0且未被占用
		*  @return 返回创建的TcpServer指针，如果创建失败返回nullptr
		*  @note 如果该流已存在TCP服务器，会返回已存在的服务器
		*  @note TCP服务器会自动处理连接事件，通过信号槽机制回调
		*  @note 该方法是线程安全的
		*  
		*  使用示例：
		*  @code
		*  auto tcp_server = service.OpenTcpServer("stream_001", 8000);
		*  if (tcp_server) {
		*      // TCP服务器创建成功，开始监听8000端口
		*  }
		*  @endcode
		*/
		libmedia_transfer_protocol::libnetwork::TcpServer*     OpenTcpServer(const std::string & stream_id, uint16_t port);




		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 新连接回调（On New Connection）
		*  
		*  该方法在TCP服务器接收到新连接时被调用。用于处理新连接的初始化工作。
		*  
		*  @param conn 新建立的连接对象指针，不能为空
		*  @note 该方法通过信号槽机制被调用
		*  @note 连接对象由TCP服务器管理，不需要手动释放
		*  
		*  使用示例：
		*  @code
		*  // 该方法由TCP服务器自动调用，无需手动调用
		*  @endcode
		*/
		void OnNewConnection(libmedia_transfer_protocol::libnetwork::Connection* conn);
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 连接销毁回调（On Destroy Connection）
		*  
		*  该方法在TCP连接被销毁时被调用。用于清理连接相关的资源。
		*  
		*  @param conn 被销毁的连接对象指针，不能为空
		*  @note 该方法通过信号槽机制被调用
		*  @note 连接对象即将被销毁，不应再使用该指针
		*  
		*  使用示例：
		*  @code
		*  // 该方法由TCP服务器自动调用，无需手动调用
		*  @endcode
		*/
		void OnDestory(libmedia_transfer_protocol::libnetwork::Connection* conn);
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 接收数据回调（On Receive Data）
		*  
		*  该方法在TCP连接接收到数据时被调用。用于处理接收到的RTP/RTCP数据包。
		*  
		*  @param conn 接收数据的连接对象指针，不能为空
		*  @param data 接收到的数据缓冲区，包含RTP/RTCP数据包
		*  @note 该方法通过信号槽机制被调用
		*  @note 数据缓冲区在回调结束后可能失效，需要及时处理
		*  
		*  使用示例：
		*  @code
		*  // 该方法由TCP服务器自动调用，无需手动调用
		*  @endcode
		*/
		void OnRecv(libmedia_transfer_protocol::libnetwork::Connection* conn, const rtc::CopyOnWriteBuffer& data);
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 发送数据回调（On Sent Data）
		*  
		*  该方法在TCP连接成功发送数据后被调用。用于统计发送状态。
		*  
		*  @param conn 发送数据的连接对象指针，不能为空
		*  @note 该方法通过信号槽机制被调用
		*  
		*  使用示例：
		*  @code
		*  // 该方法由TCP服务器自动调用，无需手动调用
		*  @endcode
		*/
		void OnSent(libmedia_transfer_protocol::libnetwork::Connection* conn);

	
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 初始化服务（Initialize Service）
		*  
		*  该方法用于初始化GB媒体服务，加载配置文件并初始化各个组件。
		*  
		*  初始化流程：
		*  1. 加载配置文件，读取服务器配置参数
		*  2. 初始化RTC服务器
		*  3. 初始化Web服务
		*  4. 初始化连接上下文
		*  
		*  @param config_file 配置文件路径，不能为空
		*  @return 如果初始化成功返回true，否则返回false
		*  @note 该方法必须在Start()之前调用
		*  @note 配置文件格式为JSON，包含服务器端口、RTC配置等信息
		*  
		*  使用示例：
		*  @code
		*  if (service.Init("config.json")) {
		*      service.Start();
		*  }
		*  @endcode
		*/
		bool Init(const char * config_file);
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 启动服务（Start Service）
		*  
		*  该方法用于启动GB媒体服务，开始监听端口并接受连接。
		*  
		*  启动流程：
		*  1. 启动RTC服务器，开始监听WebRTC连接
		*  2. 启动Web服务，开始监听HTTP API请求
		*  3. 启动所有已创建的TCP服务器
		*  
		*  @note 该方法必须在Init()之后调用
		*  @note 启动后服务会一直运行，直到调用Stop()
		*  
		*  使用示例：
		*  @code
		*  service.Init("config.json");
		*  service.Start();
		*  // 服务已启动，可以接受连接
		*  @endcode
		*/
		void Start(/*const char * ip, uint16_t port*/);
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 停止服务（Stop Service）
		*  
		*  该方法用于停止GB媒体服务，停止监听端口并断开所有连接。
		*  
		*  停止流程：
		*  1. 停止RTC服务器，断开所有WebRTC连接
		*  2. 停止Web服务，停止HTTP API服务
		*  3. 停止所有TCP服务器，断开所有TCP连接
		*  4. 关闭所有会话
		*  
		*  @note 停止后可以再次调用Start()重新启动服务
		*  @note 建议在停止前先关闭所有会话
		*  
		*  使用示例：
		*  @code
		*  service.Stop();
		*  service.Destroy();
		*  @endcode
		*/
		void Stop();
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 销毁服务（Destroy Service）
		*  
		*  该方法用于销毁GB媒体服务，释放所有资源。
		*  
		*  销毁流程：
		*  1. 确保服务已停止（如果未停止，先调用Stop()）
		*  2. 释放RTC服务器资源
		*  3. 释放Web服务资源
		*  4. 释放所有TCP服务器资源
		*  5. 清理所有会话
		*  6. 清理连接上下文
		*  
		*  @note 该方法应该在服务不再使用时调用
		*  @note 销毁后需要重新调用Init()和Start()才能再次使用
		*  
		*  使用示例：
		*  @code
		*  service.Stop();
		*  service.Destroy();
		*  @endcode
		*/
		void Destroy();

		 
#if 1
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取信令线程（Get Signaling Thread）
		*  
		*  该方法用于获取信令线程，用于处理WebRTC信令相关的操作。
		*  
		*  @return 返回信令线程指针，如果未初始化返回nullptr
		*  @note 信令线程用于处理SDP交换、ICE候选等信令操作
		*  
		*  使用示例：
		*  @code
		*  auto thread = service.signaling_thread();
		*  if (thread) {
		*      thread->PostTask([]() {
		*          // 在信令线程中执行任务
		*      });
		*  }
		*  @endcode
		*/
		rtc::Thread* signaling_thread() { return context_->signaling_thread(); }
		const rtc::Thread* signaling_thread() const { return context_->signaling_thread(); }
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取工作线程（Get Worker Thread）
		*  
		*  该方法用于获取工作线程，用于处理媒体数据处理相关的操作。
		*  
		*  @return 返回工作线程指针，如果未初始化返回nullptr
		*  @note 工作线程用于处理音视频编解码、RTP/RTCP处理等操作
		*  
		*  使用示例：
		*  @code
		*  auto thread = service.worker_thread();
		*  if (thread) {
		*      thread->PostTask([]() {
		*          // 在工作线程中执行任务
		*      });
		*  }
		*  @endcode
		*/
		rtc::Thread* worker_thread() { return context_->worker_thread(); }
		const rtc::Thread* worker_thread() const { return context_->worker_thread(); }
		
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取网络线程（Get Network Thread）
		*  
		*  该方法用于获取网络线程，用于处理网络I/O相关的操作。
		*  
		*  @return 返回网络线程指针，如果未初始化返回nullptr
		*  @note 网络线程用于处理网络数据包的接收和发送
		*  
		*  使用示例：
		*  @code
		*  auto thread = service.network_thread();
		*  if (thread) {
		*      thread->PostTask([]() {
		*          // 在网络线程中执行任务
		*      });
		*  }
		*  @endcode
		*/
		rtc::Thread* network_thread() { return context_->network_thread(); }
		const rtc::Thread* network_thread() const { return context_->network_thread(); }
#else 
		rtc::Thread* signaling_thread() { return worker_thread_.get(); }
		const rtc::Thread* signaling_thread() const { return worker_thread_.get(); }
		rtc::Thread* worker_thread() { return worker_thread_.get(); }
		const rtc::Thread* worker_thread() const { return worker_thread_.get(); }
		rtc::Thread* network_thread() { return network_thread_.get(); }
		const rtc::Thread* network_thread() const { return network_thread_.get(); }
#endif 

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取RTC服务器（Get RTC Server）
		*  
		*  该方法用于获取WebRTC服务器实例，用于处理WebRTC相关的操作。
		*  
		*  @return 返回RTC服务器指针，如果未初始化返回nullptr
		*  @note RTC服务器用于处理WebRTC推流和拉流
		*  @note 返回的指针由服务管理，不需要手动释放
		*  
		*  使用示例：
		*  @code
		*  auto rtc_server = service.GetRtcServer();
		*  if (rtc_server) {
		*      // 使用RTC服务器
		*  }
		*  @endcode
		*/
		 libmedia_transfer_protocol::librtc::RtcServer* GetRtcServer()const
		{
			return rtc_server_.get();
		}
	private:
		//EventLoopThreadPool * pool_{ nullptr };
		//std::vector<TcpServer*> servers_;
		rtc::scoped_refptr<libp2p_peerconnection::ConnectionContext>	context_;
		std::mutex lock_;
		std::unordered_map<std::string, std::shared_ptr < Session>> sessions_;

		std::unique_ptr<libmedia_transfer_protocol::librtc::RtcServer>   rtc_server_;



		std::unordered_map<std::string, std::unique_ptr<libmedia_transfer_protocol::libnetwork::TcpServer> > rtp_server_;// rtp map 
	//	std::unique_ptr<rtc::Thread>								network_thread_;
	//	std::unique_ptr<rtc::Thread>                               worker_thread_;
		//std::shared_ptr<WebrtcServer>  webrtc_server_;
		 


		std::unique_ptr< WebService>										web_service_;
	};
}


#endif // #ifndef _C_API_TRANSPORT_STUN_H_
//#define _C_API_TRANSPORT_STUN_H_