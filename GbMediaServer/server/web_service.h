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
#ifndef _C_WEB_SERVICE_H_
#define _C_WEB_SERVICE_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"

#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"

#include "libmedia_transfer_protocol/libhttp/http_context.h"
#include "libmedia_transfer_protocol/libnetwork/tcp_session.h"

namespace  gb_media_server
{
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief Web服务类（Web Service）
	 *  
	 *  该类是媒体服务器的HTTP Web服务核心类，负责处理所有HTTP请求和响应。
	 *  支持多种流媒体协议的HTTP接口，包括WebRTC、FLV、HLS(M3U8/TS)、GB28181等。
	 *  
	 *  主要功能：
	 *  1. HTTP服务器管理 - 启动和管理HTTP服务器
	 *  2. 请求路由分发 - 根据URL路径分发到不同的处理器
	 *  3. WebRTC信令处理 - 处理WebRTC推拉流的SDP协商
	 *  4. HTTP-FLV流媒体 - 提供HTTP-FLV直播流服务
	 *  5. HLS流媒体 - 提供M3U8播放列表和TS切片服务
	 *  6. GB28181接口 - 提供GB28181 RTP服务器开关接口
	 *  
	 *  支持的HTTP接口：
	 *  - /rtc/push - WebRTC推流接口
	 *  - /rtc/play - WebRTC拉流接口
	 *  - /*.flv - HTTP-FLV流媒体接口
	 *  - /*.m3u8 - HLS播放列表接口
	 *  - /*.ts - HLS TS切片接口
	 *  - /api/openRtpServer - 开启GB28181 RTP服务器
	 *  - /api/closeRtpServer - 关闭GB28181 RTP服务器
	 *  
	 *  @note 该类使用信号槽机制处理HTTP事件
	 *  @note 所有HTTP请求处理都在工作线程中异步执行
	 *  @note 支持CORS跨域请求（OPTIONS预检请求）
	 */
	class WebService  : public sigslot::has_slots<>
	{
	private:
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief HTTP事件回调函数指针类型定义
		 *  
		 *  定义了HTTP请求处理函数的统一签名，用于实现请求路由映射。
		 *  所有HTTP请求处理方法都必须符合这个函数签名。
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象
		 */
		typedef  void (WebService::*HttpEventCallback)(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);
	public:
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 构造函数
		 *  
		 *  初始化Web服务，创建HTTP服务器实例并注册所有HTTP路由。
		 *  
		 *  初始化流程：
		 *  1. 创建HTTP服务器实例
		 *  2. 连接HTTP请求和销毁信号槽
		 *  3. 注册所有HTTP路由到回调映射表
		 *  
		 *  @note 构造函数不会启动HTTP服务器，需要调用StartWebServer启动
		 */
		WebService();
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 析构函数
		 *  
		 *  清理Web服务资源，断开所有信号槽连接。
		 *  
		 *  @note 析构时会自动断开HTTP服务器的信号槽连接
		 */
		~WebService();
	
	public:
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
		 *  
		 *  使用示例：
		 *  @code
		 *  WebService webService;
		 *  // 监听所有网卡的8080端口
		 *  if (webService.StartWebServer("0.0.0.0", 8080)) {
		 *      // 服务器启动成功
		 *  }
		 *  @endcode
		 */
		bool StartWebServer(const char *ip = "127.0.0.1", uint16_t port = 8001);

	public:
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief HTTP请求处理回调（On Request）
		 *  
		 *  该方法是HTTP服务器的信号槽回调，当收到HTTP请求时触发。
		 *  负责解析HTTP请求并路由到对应的处理器。
		 *  
		 *  处理流程：
		 *  1. 从连接中获取HTTP上下文
		 *  2. 记录请求日志（方法、路径、头部等）
		 *  3. 处理OPTIONS预检请求（CORS支持）
		 *  4. 根据请求路径查找对应的处理器
		 *  5. 在工作线程中异步调用处理器
		 *  
		 *  路由匹配规则：
		 *  - 优先匹配完整路径（如 /rtc/push）
		 *  - 如果路径不匹配，则提取文件扩展名匹配（如 .flv）
		 *  - 如果都不匹配，记录警告日志
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param http_request HTTP请求对象，包含请求方法、路径、头部等
		 *  @param packet HTTP数据包对象，包含请求的原始数据
		 *  @note 该方法在HTTP服务器的IO线程中调用
		 *  @note 实际的请求处理会投递到工作线程异步执行
		 *  @note 支持CORS跨域请求，自动响应OPTIONS预检请求
		 */
		void OnRequest(libmedia_transfer_protocol::libnetwork::Connection *conn,
			const  std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> http_request, 
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet);

		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 连接销毁回调（On Destroy）
		 *  
		 *  该方法是HTTP服务器的信号槽回调，当客户端连接断开时触发。
		 *  负责清理连接相关的资源，包括生产者、消费者和编码器等。
		 *  
		 *  清理流程：
		 *  1. 从连接中获取共享资源上下文
		 *  2. 根据资源类型执行不同的清理操作：
		 *     - GB28181生产者：记录日志
		 *     - RTC/FLV/RTSP消费者：从会话中移除消费者
		 *  3. 清理共享资源上下文
		 *  4. 清理FLV编码器上下文
		 *  
		 *  资源类型说明：
		 *  - kProducerTypeGB28181：GB28181推流生产者
		 *  - kConsumerTypeRTC：WebRTC拉流消费者
		 *  - kConsumerTypeFlv：HTTP-FLV拉流消费者
		 *  - kConsumerTypeRtsp：RTSP拉流消费者
		 *  
		 *  @param conn 网络连接对象指针
		 *  @note 该方法在HTTP服务器的IO线程中调用
		 *  @note 资源清理操作会在工作线程中同步执行（Invoke）
		 *  @note 确保在连接断开时正确释放所有媒体资源
		 */
		void OnDestroy(libmedia_transfer_protocol::libnetwork::Connection *conn);
	public:
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 处理WebRTC推流请求（Handler RTC Producer）
		 *  
		 *  该方法用于处理客户端通过WebRTC推流的HTTP信令请求。
		 *  实现WebRTC的SDP Offer/Answer协商流程，建立WebRTC推流通道。
		 *  
		 *  WebRTC推流流程：
		 *  1. 客户端发送HTTP POST请求到 /rtc/push
		 *  2. 请求体包含客户端的SDP Offer
		 *  3. 服务器创建RTC生产者实例
		 *  4. 服务器生成SDP Answer并返回
		 *  5. 客户端收到Answer后建立WebRTC连接
		 *  6. 开始推送音视频流到服务器
		 *  
		 *  请求格式：
		 *  - URL: POST /rtc/push?streamurl=app/stream
		 *  - Content-Type: application/json
		 *  - Body: {"sdp": "v=0\r\no=...", "type": "offer"}
		 *  
		 *  响应格式：
		 *  - Content-Type: application/json
		 *  - Body: {"sdp": "v=0\r\no=...", "type": "answer", "code": 0}
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象，包含SDP Offer
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象，用于发送SDP Answer响应
		 *  @note 该方法在工作线程中异步执行
		 *  @note 需要从URL参数中提取streamurl作为流标识
		 *  @note 如果会话创建失败，会返回错误响应
		 *  
		 *  使用示例：
		 *  @code
		 *  // 客户端JavaScript代码
		 *  const offer = await pc.createOffer();
		 *  const response = await fetch('/rtc/push?streamurl=live/stream1', {
		 *      method: 'POST',
		 *      body: JSON.stringify({sdp: offer.sdp, type: 'offer'})
		 *  });
		 *  const answer = await response.json();
		 *  await pc.setRemoteDescription(answer);
		 *  @endcode
		 */
		void HandlerRtcProducer(libmedia_transfer_protocol::libnetwork::Connection* conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);

		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 处理WebRTC拉流请求（Handler RTC Consumer）
		 *  
		 *  该方法用于处理客户端通过WebRTC拉流的HTTP信令请求。
		 *  实现WebRTC的SDP Offer/Answer协商流程，建立WebRTC拉流通道。
		 *  
		 *  WebRTC拉流流程：
		 *  1. 客户端发送HTTP POST请求到 /rtc/play
		 *  2. 请求体包含客户端的SDP Offer
		 *  3. 服务器查找或创建对应的会话
		 *  4. 服务器创建RTC消费者实例
		 *  5. 服务器生成SDP Answer并返回
		 *  6. 客户端收到Answer后建立WebRTC连接
		 *  7. 服务器开始推送音视频流到客户端
		 *  
		 *  请求格式：
		 *  - URL: POST /rtc/play?streamurl=app/stream
		 *  - Content-Type: application/json
		 *  - Body: {"sdp": "v=0\r\no=...", "type": "offer"}
		 *  
		 *  响应格式：
		 *  - Content-Type: application/json
		 *  - Body: {"sdp": "v=0\r\no=...", "type": "answer", "code": 0}
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象，包含SDP Offer
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象，用于发送SDP Answer响应
		 *  @note 该方法在工作线程中异步执行
		 *  @note 需要从URL参数中提取streamurl作为流标识
		 *  @note 如果会话不存在或消费者创建失败，会返回错误响应
		 *  @note 消费者会自动添加到会话中，开始接收媒体流
		 *  
		 *  使用示例：
		 *  @code
		 *  // 客户端JavaScript代码
		 *  const offer = await pc.createOffer();
		 *  const response = await fetch('/rtc/play?streamurl=live/stream1', {
		 *      method: 'POST',
		 *      body: JSON.stringify({sdp: offer.sdp, type: 'offer'})
		 *  });
		 *  const answer = await response.json();
		 *  await pc.setRemoteDescription(answer);
		 *  @endcode
		 */
		void HandlerRtcConsumer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);

		

		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 处理FLV消费者HTTP请求（Handler FLV Consumer）
		*  
		*  该方法用于处理客户端通过HTTP请求FLV流的请求。当客户端请求FLV格式的流媒体时，
		*  该方法会创建FLV消费者和FLV编码器，建立HTTP-FLV推流通道。
		*  
		*  HTTP-FLV协议说明：
		*  - HTTP-FLV是基于HTTP协议的FLV流媒体传输方式
		*  - 客户端通过HTTP GET请求获取FLV流，URL格式如：http://server/live/stream1.flv
		*  - 服务器以流式方式返回FLV数据，客户端实时接收并播放
		*  - 支持实时流媒体播放，延迟较低，适合直播场景
		*  
		*  请求处理流程：
		*  1. 解析HTTP请求路径，提取流URL和会话名称
		*  2. 根据会话名称创建或查找会话
		*  3. 在会话中创建FLV消费者实例
		*  4. 创建FLV编码器，绑定到网络连接
		*  5. 将消费者添加到会话，开始接收媒体流
		*  6. 消费者会自动将媒体帧封装为FLV格式并发送
		*  
		*  URL解析说明：
		*  - 请求路径格式：/app/stream.flv
		*  - 提取会话名称格式：app/stream
		*  - 如果路径中包含扩展名（.flv），会被移除后再解析
		*  
		*  @param conn 网络连接对象指针，用于与客户端通信
		*  @param req HTTP请求对象，包含请求路径、参数等信息
		*  @param packet HTTP数据包对象，包含请求的原始数据
		*  @param ctx HTTP上下文对象，用于发送HTTP响应
		*  @note 如果会话创建失败或消费者创建失败，会返回HTTP 404响应
		*  @note 该方法会异步发送HTTP响应，不会阻塞调用线程
		*  @note FLV编码器会绑定到连接上下文中，用于后续的媒体帧封装
		*  
		*  使用示例：
		*  @code
		*  // 客户端请求示例
		*  // GET http://server/live/stream1.flv
		*  // 服务器会自动调用HandlerFlvConsumer处理该请求
		*  @endcode
		*/
		void HandlerFlvConsumer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 处理M3U8播放列表请求（Handler M3U8 Consumer）
		 *  
		 *  该方法用于处理客户端请求HLS（HTTP Live Streaming）播放列表的HTTP请求。
		 *  M3U8是HLS协议的播放列表文件，包含TS切片的URL列表和时长信息。
		 *  
		 *  HLS协议说明：
		 *  - HLS是Apple开发的HTTP流媒体传输协议
		 *  - M3U8是播放列表文件，采用UTF-8编码的M3U格式
		 *  - 播放列表包含多个TS切片的URL和时长
		 *  - 客户端按顺序下载TS切片并播放
		 *  - 支持自适应码率和实时流媒体
		 *  
		 *  请求处理流程：
		 *  1. 解析HTTP请求路径，提取流URL
		 *  2. 查找或创建对应的会话
		 *  3. 生成M3U8播放列表内容
		 *  4. 返回M3U8文件（Content-Type: application/vnd.apple.mpegurl）
		 *  
		 *  M3U8文件格式示例：
		 *  @code
		 *  #EXTM3U
		 *  #EXT-X-VERSION:3
		 *  #EXT-X-TARGETDURATION:10
		 *  #EXTINF:10.0,
		 *  stream-0.ts
		 *  #EXTINF:10.0,
		 *  stream-1.ts
		 *  @endcode
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象，包含M3U8文件路径
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象，用于发送M3U8响应
		 *  @note 该方法在工作线程中异步执行
		 *  @note M3U8文件会动态生成，包含最新的TS切片列表
		 *  @note 客户端会定期请求M3U8文件以获取最新的切片
		 *  
		 *  使用示例：
		 *  @code
		 *  // 客户端请求示例
		 *  // GET http://server/live/stream1.m3u8
		 *  // 服务器返回M3U8播放列表
		 *  @endcode
		 */
		void HandlerM3u8Consumer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 处理TS切片请求（Handler TS Consumer）
		 *  
		 *  该方法用于处理客户端请求HLS TS切片的HTTP请求。
		 *  TS（Transport Stream）是MPEG-2传输流格式，用于封装音视频数据。
		 *  
		 *  TS切片说明：
		 *  - TS是HLS协议的媒体切片格式
		 *  - 每个TS切片包含固定时长的音视频数据（通常5-10秒）
		 *  - TS切片可以独立解码播放
		 *  - 客户端按M3U8播放列表顺序下载TS切片
		 *  
		 *  请求处理流程：
		 *  1. 解析HTTP请求路径，提取TS切片文件名
		 *  2. 查找对应的会话和TS切片数据
		 *  3. 返回TS切片数据（Content-Type: video/mp2t）
		 *  4. 如果切片不存在，返回404错误
		 *  
		 *  TS文件命名规则：
		 *  - 格式：stream-{序号}.ts
		 *  - 示例：stream-0.ts, stream-1.ts, stream-2.ts
		 *  - 序号递增，表示切片的播放顺序
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象，包含TS切片文件路径
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象，用于发送TS切片响应
		 *  @note 该方法在工作线程中异步执行
		 *  @note TS切片数据会缓存在内存中，避免重复生成
		 *  @note 客户端可以并发下载多个TS切片以提高播放流畅度
		 *  
		 *  使用示例：
		 *  @code
		 *  // 客户端请求示例
		 *  // GET http://server/live/stream-0.ts
		 *  // 服务器返回TS切片数据
		 *  @endcode
		 */
		void HandlerTsConsumer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);


		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 处理开启RTP服务器请求（Handler Open RTP Server）
		 *  
		 *  该方法用于处理GB28181协议中开启RTP服务器的HTTP请求。
		 *  RTP服务器用于接收GB28181设备推送的PS流（Program Stream）。
		 *  
		 *  GB28181协议说明：
		 *  - GB28181是中国公共安全视频监控联网标准
		 *  - 使用SIP协议进行信令交互
		 *  - 使用RTP/RTCP协议传输媒体流
		 *  - 媒体流采用PS封装格式
		 *  
		 *  请求处理流程：
		 *  1. 解析HTTP请求参数（流ID、端口等）
		 *  2. 创建RTP服务器实例
		 *  3. 绑定指定的UDP端口
		 *  4. 返回RTP服务器的端口信息
		 *  5. 等待GB28181设备推流
		 *  
		 *  请求格式：
		 *  - URL: POST /api/openRtpServer
		 *  - Content-Type: application/json
		 *  - Body: {"streamId": "34020000001320000001", "port": 10000}
		 *  
		 *  响应格式：
		 *  - Content-Type: application/json
		 *  - Body: {"code": 0, "port": 10000, "msg": "success"}
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象，包含RTP服务器配置参数
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象，用于发送响应
		 *  @note 该方法在工作线程中异步执行
		 *  @note RTP服务器使用UDP协议，需要确保端口未被占用
		 *  @note 如果端口为0，系统会自动分配可用端口
		 *  @note RTP服务器会自动解析PS流并提取音视频数据
		 *  
		 *  使用示例：
		 *  @code
		 *  // 开启RTP服务器
		 *  POST /api/openRtpServer
		 *  {
		 *      "streamId": "34020000001320000001",
		 *      "port": 10000
		 *  }
		 *  @endcode
		 */
		void HandlerOpenRtpServer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 处理关闭RTP服务器请求（Handler Close RTP Server）
		 *  
		 *  该方法用于处理GB28181协议中关闭RTP服务器的HTTP请求。
		 *  关闭指定的RTP服务器并释放相关资源。
		 *  
		 *  请求处理流程：
		 *  1. 解析HTTP请求参数（流ID）
		 *  2. 查找对应的RTP服务器实例
		 *  3. 停止接收RTP数据
		 *  4. 关闭UDP端口
		 *  5. 释放RTP服务器资源
		 *  6. 返回关闭结果
		 *  
		 *  请求格式：
		 *  - URL: POST /api/closeRtpServer
		 *  - Content-Type: application/json
		 *  - Body: {"streamId": "34020000001320000001"}
		 *  
		 *  响应格式：
		 *  - Content-Type: application/json
		 *  - Body: {"code": 0, "msg": "success"}
		 *  
		 *  @param conn 网络连接对象指针
		 *  @param req HTTP请求对象，包含要关闭的RTP服务器标识
		 *  @param packet HTTP数据包对象
		 *  @param ctx HTTP上下文对象，用于发送响应
		 *  @note 该方法在工作线程中异步执行
		 *  @note 如果RTP服务器不存在，会返回错误响应
		 *  @note 关闭RTP服务器会自动断开相关的媒体流
		 *  @note 确保在不需要接收流时及时关闭RTP服务器以释放端口
		 *  
		 *  使用示例：
		 *  @code
		 *  // 关闭RTP服务器
		 *  POST /api/closeRtpServer
		 *  {
		 *      "streamId": "34020000001320000001"
		 *  }
		 *  @endcode
		 */
		void HandlerCloseRtpServer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);

		// RTSP 相关接口
	/*	void HandlerRtspProducer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);
		void HandlerRtspConsumer(libmedia_transfer_protocol::libnetwork::Connection * conn,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
			const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
			std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx);*/
	private:
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief HTTP服务器实例（HTTP Server）
		 *  
		 *  HTTP服务器的智能指针，负责监听和处理所有HTTP连接。
		 *  使用unique_ptr确保资源的唯一所有权和自动释放。
		 *  
		 *  @note 在构造函数中创建，析构函数中自动释放
		 *  @note 通过信号槽机制与WebService交互
		 */
		std::unique_ptr< libmedia_transfer_protocol::libhttp::HttpServer>  http_server_;

		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief HTTP事件回调映射表（HTTP Event Callback Map）
		 *  
		 *  存储URL路径到处理函数的映射关系，用于实现HTTP请求路由。
		 *  
		 *  映射关系说明：
		 *  - Key: URL路径或文件扩展名（如 "/rtc/push" 或 "flv"）
		 *  - Value: 成员函数指针，指向对应的处理方法
		 *  
		 *  支持的路由：
		 *  - "/rtc/push" -> HandlerRtcProducer
		 *  - "/rtc/play" -> HandlerRtcConsumer
		 *  - "flv" -> HandlerFlvConsumer
		 *  - "m3u8" -> HandlerM3u8Consumer
		 *  - "ts" -> HandlerTsConsumer
		 *  - "/api/openRtpServer" -> HandlerOpenRtpServer
		 *  - "/api/closeRtpServer" -> HandlerCloseRtpServer
		 *  
		 *  @note 在构造函数中初始化所有路由映射
		 *  @note 支持完整路径匹配和文件扩展名匹配两种方式
		 */
		std::unordered_map<std::string, HttpEventCallback>        http_event_callback_map_;

	};
}

#endif // _C_WEB_SERVICE_H_