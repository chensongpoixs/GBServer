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
				   date:  2025-10-17

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
#include "server/web_service.h"
#include "json/json.h"
#include "libmedia_transfer_protocol/libhttp/http_context.h"
#include "libmedia_transfer_protocol/libnetwork/tcp_session.h"
#include "utils/string_utils.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "utils/string_utils.h"
#include "consumer/rtc_consumer.h"
#include "producer/gb28181_producer.h"
#include "libmedia_transfer_protocol/libflv/cflv_encoder.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "gb_media_server_log.h"


namespace  gb_media_server
{
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 构造函数（Constructor）
	 *  
	 *  初始化Web服务，创建HTTP服务器实例并注册所有HTTP路由映射。
	 *  
	 *  初始化流程：
	 *  1. 创建HTTP服务器实例（http_server_）
	 *  2. 连接HTTP请求信号到OnRequest方法
	 *  3. 连接连接销毁信号到OnDestroy方法
	 *  4. 注册所有HTTP路由到回调映射表：
	 *     - WebRTC推流接口：/rtc/push
	 *     - WebRTC拉流接口：/rtc/play
	 *     - HTTP-FLV接口：.flv扩展名
	 *     - HLS M3U8接口：.m3u8扩展名
	 *     - HLS TS接口：.ts扩展名
	 *     - GB28181 RTP服务器开启接口：/api/openRtpServer
	 *     - GB28181 RTP服务器关闭接口：/api/closeRtpServer
	 *  
	 *  @note 构造函数不会启动HTTP服务器，需要调用StartWebServer启动
	 *  @note 使用信号槽机制实现HTTP事件的异步处理
	 *  @note 路由映射表支持完整路径匹配和文件扩展名匹配
	 */
	WebService::WebService()
		:http_server_(new libmedia_transfer_protocol::libhttp::HttpServer())
		, http_event_callback_map_()
	{
		http_server_->SignalOnRequest.connect(this, &WebService::OnRequest);
		http_server_->SignalOnDestory.connect(this, &WebService::OnDestroy);
		http_event_callback_map_["/rtc/push"] = &WebService::HandlerRtcProducer;
		// http_event_callback_map_["/rtc/pull"] = &WebService::HandlerRtcConsumer;
		http_event_callback_map_["/rtc/play"] = &WebService::HandlerRtcConsumer;
		// flv 
		http_event_callback_map_["flv"] = &WebService::HandlerFlvConsumer;

		// m3u8
		http_event_callback_map_["m3u8"] = &WebService::HandlerM3u8Consumer;

		// ts
		http_event_callback_map_["ts"] = &WebService::HandlerTsConsumer;

		//// rtp 
		http_event_callback_map_["/api/openRtpServer"] = &WebService::HandlerOpenRtpServer;
		http_event_callback_map_["/api/closeRtpServer"] = &WebService::HandlerCloseRtpServer;
		
		// 统计接口
		http_event_callback_map_["/api/stats/producer"] = &WebService::HandlerGetProducerStats;
		http_event_callback_map_["/api/stats/consumer"] = &WebService::HandlerGetConsumerStats;
		http_event_callback_map_["/api/stats/session"] = &WebService::HandlerGetSessionStats;
		http_event_callback_map_["/api/stats/sessions"] = &WebService::HandlerGetAllSessionsStats;
		
		// RTSP
		//    创建 RTSP Producer（接收 RTSP 流）：
			// POST /rtsp/push?streamurl=app/stream
			// 创建 RTSP Consumer（发送 RTSP 流）：
           //    POST /rtsp/play?streamurl=app/stream
		//http_event_callback_map_["/rtsp/push"] = &WebService::HandlerRtspProducer;
		// http_event_callback_map_["/rtsp/pull"] = &WebService::HandlerRtspConsumer;
		//http_event_callback_map_["/rtsp/play"] = &WebService::HandlerRtspConsumer;
	
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 析构函数（Destructor）
	 *  
	 *  清理Web服务资源，断开所有信号槽连接。
	 *  
	 *  清理流程：
	 *  1. 检查HTTP服务器实例是否存在
	 *  2. 断开OnRequest信号槽连接
	 *  3. 断开OnDestroy信号槽连接
	 *  
	 *  @note HTTP服务器实例使用unique_ptr管理，会自动释放
	 *  @note 断开信号槽连接可以避免悬空指针问题
	 */
	WebService::~WebService()
	{
		if (http_server_)
		{
			http_server_->SignalOnRequest.disconnect(this);
			http_server_->SignalOnDestory.disconnect(this);
		}
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 启动Web服务器（Start Web Server）
	 *  
	 *  启动HTTP服务器并监听指定的IP地址和端口。
	 *  
	 *  @param ip 监听的IP地址，默认为"127.0.0.1"（本地回环地址）
	 *  @param port 监听的端口号，默认为8001
	 *  @return bool 启动成功返回true，失败返回false
	 *  @note 如果端口已被占用，启动会失败
	 *  @note 建议在生产环境中使用"0.0.0.0"监听所有网卡
	 *  @note 该方法会阻塞直到服务器启动完成
	 */
	bool WebService::StartWebServer(const char * ip, uint16_t port)
	{
		return http_server_->Startup(ip, port);
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief HTTP请求处理回调（On Request）
	 *  
	 *  该方法是HTTP服务器的信号槽回调，当收到HTTP请求时触发。
	 *  负责解析HTTP请求、记录日志并路由到对应的处理器。
	 *  
	 *  处理流程：
	 *  1. 从连接中获取HTTP上下文，如果不存在则记录警告并返回
	 *  2. 记录请求日志：
	 *     - 如果是请求：记录方法（GET/POST等）和路径
	 *     - 如果是响应：记录状态码和状态消息
	 *  3. 记录所有HTTP头部信息
	 *  4. 处理OPTIONS预检请求（CORS支持）：
	 *     - 创建OPTIONS响应
	 *     - 发送响应并完成写入
	 *  5. 对于其他请求，投递到工作线程异步处理：
	 *     - 优先匹配完整路径（如 /rtc/push）
	 *     - 如果路径不匹配，提取文件扩展名匹配（如 .flv）
	 *     - 调用对应的处理器方法
	 *     - 如果都不匹配，记录警告日志
	 *  
	 *  CORS支持说明：
	 *  - OPTIONS请求用于浏览器的跨域预检
	 *  - 服务器返回允许的方法、头部等信息
	 *  - 预检通过后，浏览器才会发送实际请求
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象，包含请求方法、路径、头部等
	 *  @param packet HTTP数据包对象，包含请求的原始数据
	 *  @note 该方法在HTTP服务器的IO线程中调用
	 *  @note 实际的请求处理会投递到工作线程异步执行，避免阻塞IO线程
	 *  @note 使用成员函数指针实现路由分发，提高代码可维护性
	 */
	void WebService::OnRequest(libmedia_transfer_protocol::libnetwork::Connection * conn,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet)
	{
		std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext>  http_ctx = 
			conn->GetContext<libmedia_transfer_protocol::libhttp::HttpContext>(libmedia_transfer_protocol::libnetwork::kHttpContext);
		if (!http_ctx)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "no found http context.something must be wrong.";
			return;
		}
		if (req->IsRequest())
		{
			GBMEDIASERVER_LOG(LS_INFO) << "req method:" << req->Method() << " path:" << req->Path();
		}
		else
		{
			GBMEDIASERVER_LOG(LS_INFO) << "req code:" << req->GetStatusCode() << " msg:" << libmedia_transfer_protocol::libhttp::HttpUtils::ParseStatusMessage(req->GetStatusCode());
		}

		auto headers = req->Headers();
		for (auto const &h : headers)
		{
			GBMEDIASERVER_LOG(LS_INFO) << h.first << ":" << h.second;
		}

		if (req->IsRequest())
		{
			if (req->Method() == libmedia_transfer_protocol::libhttp::kOptions)
			{
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttpOptionsResponse();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
				return;
			}
			GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [=]() {

				auto event_iter = http_event_callback_map_.find(req->Path());
				if (event_iter != http_event_callback_map_.end())
				{
					(this->*(event_iter->second))(conn, req, packet, http_ctx);
				} 
				else
				{
					std::string ext = string_utils::FileExt(req->Path());
					GBMEDIASERVER_LOG(LS_INFO) << "ext:" << ext;
					event_iter = http_event_callback_map_.find(ext);
					if (event_iter != http_event_callback_map_.end())
					{
						(this->*(event_iter->second))(conn, req, packet, http_ctx);
					}
					else
					{
						RTC_LOG(LS_WARNING) << "http  event callback path:" << req->Path();
					}
				}

					
 

			}); 
		}
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 连接销毁回调（On Destroy）
	 *  
	 *  该方法是HTTP服务器的信号槽回调，当客户端连接断开时触发。
	 *  负责清理连接相关的所有资源，包括生产者、消费者和编码器等。
	 *  
	 *  清理流程：
	 *  1. 从连接中获取共享资源上下文（ShareResource）
	 *  2. 如果共享资源存在，根据资源类型执行不同的清理操作：
	 *     - kProducerTypeGB28181（GB28181生产者）：
	 *       记录警告日志，标识生产者资源类型
	 *     - kConsumerTypeRTC（WebRTC消费者）：
	 *       从会话中移除该消费者，停止推送媒体流
	 *     - kConsumerTypeFlv（HTTP-FLV消费者）：
	 *       从会话中移除该消费者，停止推送FLV流
	 *     - kConsumerTypeRtsp（RTSP消费者）：
	 *       从会话中移除该消费者，停止推送RTSP流
	 *     - 其他类型：
	 *       记录警告日志，标识未知资源类型
	 *  3. 在工作线程中同步清理共享资源上下文
	 *  4. 从连接中获取FLV编码器上下文（FlvEncoder）
	 *  5. 如果FLV编码器存在，在工作线程中同步清理编码器上下文
	 *  
	 *  资源类型说明：
	 *  - kProducerTypeGB28181：GB28181推流生产者，接收GB28181设备的PS流
	 *  - kConsumerTypeRTC：WebRTC拉流消费者，向客户端推送WebRTC流
	 *  - kConsumerTypeFlv：HTTP-FLV拉流消费者，向客户端推送FLV流
	 *  - kConsumerTypeRtsp：RTSP拉流消费者，向客户端推送RTSP流
	 *  
	 *  @param conn 网络连接对象指针
	 *  @note 该方法在HTTP服务器的IO线程中调用
	 *  @note 资源清理操作使用Invoke在工作线程中同步执行，确保线程安全
	 *  @note 消费者从会话中移除后，会自动停止接收和推送媒体流
	 *  @note FLV编码器的清理是独立的，因为编码器可能被多个消费者共享
	 */
	void WebService::OnDestroy(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		auto shareResource = conn->GetContext<ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
		if (shareResource)
		{
			switch (shareResource->ShareResouceType())
			{
			case kProducerTypeGB28181:
			{
				//shareResource->GetSession()->SetProducer(nullptr);
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << " producer  share resource type :" << shareResource->ShareResouceType();
				break;
			}
			case kConsumerTypeRTC:
			case kConsumerTypeFlv:
			case kConsumerTypeRtsp:
			{
				shareResource->GetSession()->RemoveConsumer(std::dynamic_pointer_cast<Consumer>(shareResource));
				break;
			}
			default:
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << " share resource type :" << shareResource->ShareResouceType();
				break;
			}
			gb_media_server::GbMediaService::GetInstance().worker_thread()->Invoke<void>(RTC_FROM_HERE, [&]() {
				conn->ClearContext(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
			});
			
			 
		}
		auto flv = conn->GetContext<libmedia_transfer_protocol::libflv::FlvEncoder>(libmedia_transfer_protocol::libnetwork::kFlvContext);
		if (flv)
		{ 
			gb_media_server::GbMediaService::GetInstance().worker_thread()->Invoke<void>(RTC_FROM_HERE, [& ]() {
				conn->ClearContext(libmedia_transfer_protocol::libnetwork::kFlvContext);
			});
			 
		}
	}
	
	//void WebService::HandlerRtcPlay(libmedia_transfer_protocol::libhttp::TcpSession * conn,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
	//	std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx)
	//{
	//}
}
