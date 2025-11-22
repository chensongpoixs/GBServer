 /*****************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *
 *
 *				   Author: chensong
 *				   date:  2025-10-14
 *
 * 输赢不重要，答案对你们有什么意义才重要。
 *
 * 光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。
 *
 *
 *我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
 *然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
 *3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
 *然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
 *于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
 *我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
 *从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
 *我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
 *沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
 *安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
 *
 *******************************************************************************/
#ifndef _C_RTC_SERVICE_H_
#define _C_RTC_SERVICE_H_

#include "server/session.h" 
#include "server/session.h"
#include "server/stream.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"
#include "producer/gb28181_producer.h"
#include "consumer/rtc_consumer.h"
//#include "swagger/dto/RtcApiDto.hpp"
#include "share/rtc_interface.h"
namespace  gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief RTC服务类（RTC Service）
	*  
	*  RtcService是GBMediaServer流媒体服务器中用于管理所有RTC相关连接和数据的服务类。
	*  它采用单例模式，负责处理来自RTC服务器的各种数据包（STUN、DTLS、RTP、RTCP），
	*  并将这些数据包路由到对应的RTC接口（RtcProducer或RtcConsumer）。
	*  
	*  RTC服务功能：
	*  1. 管理所有RTC接口（RtcInterface）的注册和注销
	*  2. 接收来自RTC服务器的数据包（STUN、DTLS、RTP、RTCP）
	*  3. 根据远程地址或会话名称路由数据包到对应的RTC接口
	*  4. 提供任务队列工厂（TaskQueueFactory）供RTC接口使用
	*  5. 支持UDP和TCP两种传输协议
	*  
	*  数据包路由流程：
	*  1. RTC服务器接收到数据包后，调用RtcService的对应方法（OnStun/OnDtls/OnRtp/OnRtcp）
	*  2. RtcService根据数据包的远程地址查找对应的RTC接口
	*  3. 将数据包转发到找到的RTC接口进行处理
	*  4. RTC接口根据协议类型进行相应的处理（解密、解析等）
	*  
	*  @note RtcService采用单例模式，确保整个系统中只有一个实例
	*  @note 所有方法都是线程安全的，使用互斥锁保护共享数据
	*  @note 支持同时处理多个RTC连接
	*  
	*  使用示例：
	*  @code
	*  // 获取单例实例
	*  RtcService& service = RtcService::GetInstance();
	*  
	*  // 添加RTC接口
	*  service.AddConsumer(rtc_consumer);
	*  
	*  // 移除RTC接口
	*  service.RemoveConsumer(rtc_consumer);
	*  @endcode
	*/
	class RtcService : public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化RtcService实例。它会创建任务队列工厂，
		*  用于为RTC接口提供异步任务执行能力。
		*  
		*  初始化流程：
		*  1. 创建TaskQueueFactory实例，用于创建任务队列
		*  2. 初始化内部数据结构（映射表、互斥锁等）
		*  
		*  @note 构造函数会自动创建任务队列工厂
		*  @note 任务队列工厂用于RTC接口的异步操作
		*/
		RtcService() ;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtcService实例。它会释放所有相关资源，
		*  包括任务队列工厂和已注册的RTC接口。
		*  
		*  清理流程：
		*  1. 清空所有RTC接口映射
		*  2. 释放任务队列工厂
		*  3. 清理所有资源
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual	~RtcService() ;
	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取单例实例（Get Instance）
		*  
		*  该方法用于获取RtcService的单例实例。采用线程安全的单例模式实现，
		*  确保整个系统中只有一个RtcService实例。
		*  
		*  @return 返回RtcService单例实例的引用
		*  @note 该方法线程安全，使用C++11的静态局部变量实现单例模式
		*  @note 单例实例在第一次调用时创建，程序结束时自动销毁
		*  
		*  使用示例：
		*  @code
		*  RtcService& service = RtcService::GetInstance();
		*  service.AddConsumer(rtc_consumer);
		*  @endcode
		*/
		static RtcService & GetInstance()
		{
			static RtcService   instance;
			return instance;
		}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 添加RTC接口（Add RTC Interface）
		*  
		*  该方法用于将RTC接口（RtcProducer或RtcConsumer）注册到RTC服务中。
		*  注册后，RTC服务可以根据远程地址或会话名称将数据包路由到对应的接口。
		*  
		*  注册流程：
		*  1. 获取RTC接口的远程地址和会话名称
		*  2. 将接口添加到映射表中（按远程地址和会话名称索引）
		*  3. 使用互斥锁保护共享数据，确保线程安全
		*  
		*  @param rtc_interface 指向RTC接口的共享指针，不能为空
		*  @note 该方法线程安全，使用互斥锁保护映射表
		*  @note 同一个RTC接口可以按多个键（地址、会话名）注册
		*  @note 注册后，该接口就可以接收来自RTC服务器的数据包
		*  
		*  使用示例：
		*  @code
		*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
		*  RtcService::GetInstance().AddConsumer(consumer);
		*  @endcode
		*/
		void AddConsumer(std::shared_ptr<RtcInterface>  rtc_interface);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 移除RTC接口（Remove RTC Interface）
		*  
		*  该方法用于从RTC服务中注销RTC接口。注销后，RTC服务将不再
		*  向该接口路由数据包。
		*  
		*  注销流程：
		*  1. 从映射表中查找并移除该RTC接口
		*  2. 移除所有与该接口相关的索引（地址、会话名）
		*  3. 使用互斥锁保护共享数据，确保线程安全
		*  
		*  @param rtc_interface 指向RTC接口的共享指针
		*  @note 该方法线程安全，使用互斥锁保护映射表
		*  @note 如果接口不存在，操作将被忽略
		*  
		*  使用示例：
		*  @code
		*  RtcService::GetInstance().RemoveConsumer(rtc_consumer);
		*  @endcode
		*/
		void RemoveConsumer(std::shared_ptr<RtcInterface> rtc_interface);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取任务队列工厂（Get Task Queue Factory）
		*  
		*  该方法用于获取任务队列工厂实例。任务队列工厂用于创建任务队列，
		*  供RTC接口执行异步操作（如发送RTP包、处理RTCP包等）。
		*  
		*  任务队列说明：
		*  - 任务队列用于执行异步任务，避免阻塞主线程
		*  - 每个RTC接口可以使用独立的任务队列
		*  - 任务队列工厂由WebRTC库提供
		*  
		*  @return 返回指向任务队列工厂的指针，如果未初始化则返回nullptr
		*  @note 任务队列工厂在构造函数中创建
		*  @note 多个RTC接口可以共享同一个任务队列工厂
		*  
		*  使用示例：
		*  @code
		*  auto factory = RtcService::GetInstance().GetTaskQueueFactory();
		*  auto task_queue = factory->CreateTaskQueue("RTC", webrtc::TaskQueueFactory::Priority::NORMAL);
		*  @endcode
		*/
		webrtc::TaskQueueFactory*  GetTaskQueueFactory();
	//	oatpp::Object<RtcApiDto>   CreateOfferAnswer(const oatpp::Object<RtcApiDto>& dto);
	public:

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理STUN数据包（On STUN Packet）
		*  
		*  该方法用于处理来自UDP套接字的STUN数据包。STUN（Session Traversal Utilities for NAT）
		*  用于NAT穿透和ICE连接建立。该方法会将STUN数据包路由到对应的RTC接口。
		*  
		*  处理流程：
		*  1. 根据数据包的远程地址查找对应的RTC接口
		*  2. 将STUN数据包转发到找到的RTC接口
		*  3. RTC接口处理STUN数据包（NAT穿透、ICE候选交换等）
		*  
		*  STUN协议说明：
		*  - STUN用于NAT穿透，帮助客户端发现自己的公网IP和端口
		*  - STUN用于ICE（Interactive Connectivity Establishment）连接建立
		*  - STUN数据包通常很小（几十到几百字节）
		*  
		*  @param socket UDP异步数据包套接字指针，用于接收数据包
		*  @param data STUN数据包的指针，包含完整的STUN消息
		*  @param len STUN数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note 如果找不到对应的RTC接口，STUN数据包将被丢弃
		*  
		*  使用示例：
		*  @code
		*  // 在网络线程中调用
		*  service.OnStun(socket, data, len, remote_addr, timestamp);
		*  @endcode
		*/
		void OnStun(rtc::AsyncPacketSocket* socket,
			const uint8_t * data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理DTLS数据包（On DTLS Packet）
		*  
		*  该方法用于处理来自UDP套接字的DTLS数据包。DTLS（Datagram Transport Layer Security）
		*  用于WebRTC的密钥交换和握手过程。该方法会将DTLS数据包路由到对应的RTC接口。
		*  
		*  处理流程：
		*  1. 根据数据包的远程地址查找对应的RTC接口
		*  2. 将DTLS数据包转发到找到的RTC接口
		*  3. RTC接口处理DTLS数据包（握手、证书交换、密钥协商等）
		*  
		*  DTLS协议说明：
		*  - DTLS用于在UDP上提供TLS安全连接
		*  - DTLS用于WebRTC的密钥交换，建立SRTP加密会话
		*  - DTLS握手过程包括客户端Hello、服务器Hello、证书交换、密钥交换等
		*  
		*  @param socket UDP异步数据包套接字指针，用于接收数据包
		*  @param data DTLS数据包的指针，包含DTLS记录
		*  @param len DTLS数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note 如果找不到对应的RTC接口，DTLS数据包将被丢弃
		*  @note DTLS握手通常需要多次往返，需要持续处理
		*  
		*  使用示例：
		*  @code
		*  // 在网络线程中调用
		*  service.OnDtls(socket, data, len, remote_addr, timestamp);
		*  @endcode
		*/
		void OnDtls(rtc::AsyncPacketSocket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理RTP数据包（On RTP Packet）
		*  
		*  该方法用于处理来自UDP套接字的RTP数据包。RTP（Real-time Transport Protocol）
		*  用于传输音视频媒体数据。该方法会将RTP数据包路由到对应的RTC接口。
		*  
		*  处理流程：
		*  1. 根据数据包的远程地址查找对应的RTC接口
		*  2. 将RTP数据包转发到找到的RTC接口
		*  3. RTC接口处理RTP数据包（解密、解析、重构媒体帧等）
		*  
		*  RTP协议说明：
		*  - RTP用于传输实时音视频数据
		*  - RTP包包含序列号、时间戳、SSRC等头部信息
		*  - RTP包通常经过SRTP加密后传输
		*  - RTP包可能被分片，需要重组为完整的媒体帧
		*  
		*  @param socket UDP异步数据包套接字指针，用于接收数据包
		*  @param data RTP数据包的指针，可能包含SRTP加密的数据
		*  @param len RTP数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note 如果找不到对应的RTC接口，RTP数据包将被丢弃
		*  @note RTP包可能需要解密（SRTP）后才能解析
		*  
		*  使用示例：
		*  @code
		*  // 在网络线程中调用
		*  service.OnRtp(socket, data, len, remote_addr, timestamp);
		*  @endcode
		*/
		void OnRtp(rtc::AsyncPacketSocket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理RTCP数据包（On RTCP Packet）
		*  
		*  该方法用于处理来自UDP套接字的RTCP数据包。RTCP（RTP Control Protocol）
		*  用于传输控制信息（接收报告、发送报告、反馈等）。该方法会将RTCP数据包路由到对应的RTC接口。
		*  
		*  处理流程：
		*  1. 根据数据包的远程地址查找对应的RTC接口
		*  2. 将RTCP数据包转发到找到的RTC接口
		*  3. RTC接口处理RTCP数据包（统计、反馈、关键帧请求等）
		*  
		*  RTCP协议说明：
		*  - RTCP用于传输控制信息，与RTP配合使用
		*  - RTCP包类型包括：接收报告（RR）、发送报告（SR）、反馈（PLI/FIR/NACK）等
		*  - RTCP包通常经过SRTCP加密后传输
		*  - RTCP包用于统计传输质量、请求关键帧、反馈接收状态等
		*  
		*  @param socket UDP异步数据包套接字指针，用于接收数据包
		*  @param data RTCP数据包的指针，可能包含SRTCP加密的数据
		*  @param len RTCP数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note 如果找不到对应的RTC接口，RTCP数据包将被丢弃
		*  @note RTCP包可能需要解密（SRTCP）后才能解析
		*  
		*  使用示例：
		*  @code
		*  // 在网络线程中调用
		*  service.OnRtcp(socket, data, len, remote_addr, timestamp);
		*  @endcode
		*/
		void OnRtcp(rtc::AsyncPacketSocket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;


	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理STUN数据包（TCP版本）（On STUN Packet - TCP）
		*  
		*  该方法用于处理来自TCP套接字的STUN数据包。功能与UDP版本的OnStun相同，
		*  但使用TCP套接字而不是UDP套接字。
		*  
		*  @param socket TCP套接字指针，用于接收数据包
		*  @param data STUN数据包的指针，包含完整的STUN消息
		*  @param len STUN数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note TCP版本的STUN处理较少使用，主要用于某些特殊场景
		*/
		void OnStun(rtc::Socket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理DTLS数据包（TCP版本）（On DTLS Packet - TCP）
		*  
		*  该方法用于处理来自TCP套接字的DTLS数据包。功能与UDP版本的OnDtls相同，
		*  但使用TCP套接字而不是UDP套接字。
		*  
		*  @param socket TCP套接字指针，用于接收数据包
		*  @param data DTLS数据包的指针，包含DTLS记录
		*  @param len DTLS数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note TCP版本的DTLS处理较少使用，主要用于某些特殊场景
		*/
		void OnDtls(rtc::Socket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理RTP数据包（TCP版本）（On RTP Packet - TCP）
		*  
		*  该方法用于处理来自TCP套接字的RTP数据包。功能与UDP版本的OnRtp相同，
		*  但使用TCP套接字而不是UDP套接字。TCP传输RTP主要用于某些特殊场景，
		*  如防火墙限制UDP等。
		*  
		*  @param socket TCP套接字指针，用于接收数据包
		*  @param data RTP数据包的指针，可能包含SRTP加密的数据
		*  @param len RTP数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note TCP版本的RTP处理较少使用，主要用于某些特殊场景
		*/
		void OnRtp(rtc::Socket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 处理RTCP数据包（TCP版本）（On RTCP Packet - TCP）
		*  
		*  该方法用于处理来自TCP套接字的RTCP数据包。功能与UDP版本的OnRtcp相同，
		*  但使用TCP套接字而不是UDP套接字。
		*  
		*  @param socket TCP套接字指针，用于接收数据包
		*  @param data RTCP数据包的指针，可能包含SRTCP加密的数据
		*  @param len RTCP数据包的长度，单位为字节
		*  @param addr 数据包的远程地址（发送方地址）
		*  @param ms 数据包接收时间戳，单位为毫秒
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note TCP版本的RTCP处理较少使用，主要用于某些特殊场景
		*/
		void OnRtcp(rtc::Socket* socket,
			const uint8_t* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms);
	public:
		 
		
	public:


	//	static std::string GetSessionNameFromUrl(const std::string &url);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 互斥锁（Mutex Lock）
		*  
		*  该互斥锁用于保护name_rtc_interface_映射表的并发访问。
		*  确保在多线程环境下对映射表的操作是线程安全的。
		*  
		*  @note 所有对name_rtc_interface_的访问都需要先获取此锁
		*  @note 使用std::lock_guard自动管理锁的获取和释放
		*/
		std::mutex lock_;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief RTC接口映射表（按会话名称索引）（RTC Interface Map by Session Name）
		*  
		*  该映射表用于根据会话名称快速查找对应的RTC接口。
		*  会话名称格式通常为 "app/stream"。
		*  
		*  映射表说明：
		*  - Key: 会话名称字符串（如 "live/stream1"）
		*  - Value: 指向RTC接口的共享指针（RtcProducer或RtcConsumer）
		*  
		*  @note 该映射表由lock_互斥锁保护
		*  @note 一个会话名称可以对应多个RTC接口（一个Producer和多个Consumer）
		*  @note 使用会话名称作为键，便于根据URL路由数据包
		*/
		std::unordered_map<std::string, std::shared_ptr<RtcInterface>> name_rtc_interface_;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief RTC接口互斥锁（RTC Interface Mutex Lock）
		*  
		*  该互斥锁用于保护rtc_interfaces_映射表的并发访问。
		*  确保在多线程环境下对映射表的操作是线程安全的。
		*  
		*  @note 所有对rtc_interfaces_的访问都需要先获取此锁
		*  @note 使用std::lock_guard自动管理锁的获取和释放
		*/
		std::mutex rtcinterface_lock_;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief RTC接口映射表（按远程地址索引）（RTC Interface Map by Remote Address）
		*  
		*  该映射表用于根据远程地址快速查找对应的RTC接口。
		*  远程地址包含IP和端口信息，用于UDP数据包路由。
		*  
		*  映射表说明：
		*  - Key: 远程地址字符串（格式为 "IP:Port"）
		*  - Value: 指向RTC接口的共享指针（RtcProducer或RtcConsumer）
		*  
		*  @note 该映射表由rtcinterface_lock_互斥锁保护
		*  @note 一个远程地址通常只对应一个RTC接口
		*  @note 使用远程地址作为键，便于根据UDP包的目标地址路由数据包
		*/
		std::unordered_map<std::string, std::shared_ptr<RtcInterface>> rtc_interfaces_;
	public:
	private:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 任务队列工厂（Task Queue Factory）
		*  
		*  该任务队列工厂用于创建任务队列，供RTC接口执行异步操作。
		*  任务队列工厂由WebRTC库提供，支持创建具有不同优先级的任务队列。
		*  
		*  任务队列说明：
		*  - 任务队列用于执行异步任务，避免阻塞主线程
		*  - 支持不同优先级的任务队列（NORMAL、HIGH、LOW等）
		*  - 每个RTC接口可以使用独立的任务队列
		*  
		*  @note 任务队列工厂在构造函数中创建
		*  @note 使用std::unique_ptr管理生命周期，自动释放资源
		*  @note 多个RTC接口可以共享同一个任务队列工厂
		*/
		std::unique_ptr<webrtc::TaskQueueFactory>                       task_queue_factory_;
	};
}


#endif // 