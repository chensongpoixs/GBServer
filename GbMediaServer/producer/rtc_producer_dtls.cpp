/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
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
 
#include <random>
#include "consumer/rtc_consumer.h"
#include "server/session.h"
#include "server/stream.h"
 
#include "rtc_base/buffer.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format_h264.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include <vector>
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format.h"
#include <memory>
#include "server/rtc_service.h"
#include "common_video/h264/h264_common.h"
#include "libmedia_transfer_protocol/librtc/rtc_errors.h"
#include "producer/rtc_producer.h"
#include "gb_media_server_log.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"


namespace gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief DTLS连接中回调（On DTLS Connecting）
	*  
	*  该方法在DTLS握手开始时被调用。它用于通知RTC生产者DTLS连接正在建立中。
	*  
	*  DTLS握手流程：
	*  1. OnDtlsConnecting: DTLS握手开始
	*  2. 交换ClientHello和ServerHello
	*  3. 交换证书和密钥
	*  4. OnDtlsConnected: DTLS握手完成
	*  
	*  @param dtls 指向DTLS对象的指针
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 当前版本只记录日志，未进行实际处理
	*/
	 void RtcProducer::OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO) << "DTLS connecting" ;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief DTLS连接完成回调（On DTLS Connected）
	*  
	*  该方法在DTLS握手完成时被调用。它会创建SRTP会话，用于加密和解密RTP/RTCP数据包。
	*  如果SDP中包含DataChannel参数，还会创建SCTP关联，用于传输DataChannel数据。
	*  
	*  处理流程：
	*  1. 记录日志，标记DTLS连接完成
	*  2. 删除旧的SRTP会话（如果存在）
	*  3. 创建新的SRTP发送会话（使用本地密钥）
	*  4. 创建新的SRTP接收会话（使用远程密钥）
	*  5. 设置dtls_done_标志为true
	*  6. 如果SDP包含DataChannel，创建SCTP关联
	*  7. 启动定时器，定期发送RTCP RR报告
	*  
	*  SRTP会话说明：
	*  - SRTP（Secure RTP）用于加密RTP数据包
	*  - SRTCP（Secure RTCP）用于加密RTCP数据包
	*  - 发送会话使用本地密钥加密
	*  - 接收会话使用远程密钥解密
	*  
	*  @param dtls 指向DTLS对象的指针
	*  @param srtpCryptoSuite SRTP加密套件（如AES_CM_128_HMAC_SHA1_80）
	*  @param srtpLocalKey 本地SRTP密钥
	*  @param srtpLocalKeyLen 本地SRTP密钥长度
	*  @param srtpRemoteKey 远程SRTP密钥
	*  @param srtpRemoteKeyLen 远程SRTP密钥长度
	*  @param remote_cert 远程证书（用于验证）
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 如果创建SRTP会话失败，会记录错误日志
	*  @note DTLS握手完成后，才能开始发送和接收RTP/RTCP数据
	*/
	void RtcProducer::OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
		libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
		uint8_t* srtpLocalKey,
		size_t srtpLocalKeyLen,
		uint8_t* srtpRemoteKey,
		size_t srtpRemoteKeyLen,
		std::string& remote_cert)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "DTLS connected";

		// 删除旧的SRTP会话
		delete srtp_send_session_;
		srtp_send_session_ = nullptr;
		delete srtp_recv_session_;
		srtp_recv_session_ = nullptr;

		// 创建SRTP发送会话（使用本地密钥）
		try
		{
			srtp_send_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::OUTBOUND, srtpCryptoSuite, srtpLocalKey, srtpLocalKeyLen);
		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR)<< "error creating SRTP sending session: " <<  error.what();
		}
		
		// 创建SRTP接收会话（使用远程密钥）
		try
		{
			srtp_recv_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::INBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);
 
		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR) << "error creating SRTP receiving session: %s", error.what();

			// 如果接收会话创建失败，删除发送会话
			delete srtp_send_session_;
			srtp_send_session_ = nullptr;
		}
		
		// 设置DTLS完成标志
		dtls_done_ = true;
		
		// 如果SDP包含DataChannel，创建SCTP关联
		if (sdp_.GetDataChannelParams().application)
		{
			CreateDataChannel();
		}

		// 启动定时器，定期发送RTCP RR报告
		if (dtls_done_)
		{
			gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(ToQueuedTask(task_safety_,
				[this]() {
					if (!dtls_done_)
					{ 
						return;
					}
					// 更新RTCP RR时间戳
					rtcp_rr_timestamp_ = rtc::SystemTimeMillis();
					
					// 创建RTCP RR报告
					rtc::Buffer buffer = rtcp_context_recv_->createRtcpRR(sdp_.VideoSsrc(), sdp_.VideoSsrc());

					// 发送RTCP RR报告
					SendSrtpRtcp(buffer.data(), buffer.size());

					// 启动定时器
					OnTimer();
				}) );
		}

		 
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief DTLS发送数据包回调（On DTLS Send Packet）
	*  
	*  该方法在DTLS需要发送数据包时被调用。它会将DTLS数据包通过UDP套接字发送到远程地址。
	*  
	*  处理流程：
	*  1. 记录日志，标记DTLS发送数据包
	*  2. 将数据包封装为rtc::Buffer
	*  3. 通过RTC服务器发送数据包到远程地址
	*  
	*  @param dtls 指向DTLS对象的指针
	*  @param data DTLS数据包的指针
	*  @param len DTLS数据包的长度
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note DTLS数据包通过UDP套接字发送
	*/
	void RtcProducer::OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t *data, size_t len)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "dtls send size:" << len;
		 
		// 封装为rtc::Buffer
		rtc::Buffer buffer(data, len);
		
		// 通过RTC服务器发送数据包
		GbMediaService::GetInstance().GetRtcServer()->SendPacketTo(std::move(buffer), rtc_remote_address_, rtc::PacketOptions());
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief DTLS连接关闭回调（On DTLS Closed）
	*  
	*  该方法在DTLS连接被远程端关闭时被调用。它会清理所有资源，
	*  并从RTC服务中注销RTC接口。
	*  
	*  处理流程：
	*  1. 记录警告日志，标记DTLS连接关闭
	*  2. 设置dtls_done_标志为false
	*  3. 获取会话名称
	*  4. 在工作线程中执行清理操作：
	*     a. 从RTC服务中注销RTC接口
	*     b. 清空Session的Producer
	*  
	*  @param dtls 指向DTLS对象的指针
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 清理操作在工作线程中执行，避免阻塞网络线程
	*  @note DTLS连接关闭后，无法再发送和接收RTP/RTCP数据
	*/
	void RtcProducer::OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls *dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS remotely closed";
		dtls_done_ = false;

		// 获取会话名称
		std::string session_name = GetSession()->SessionName();
		
		// 在工作线程中执行清理操作
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			// 从RTC服务中注销RTC接口
			std::shared_ptr<RtcProducer> slef = std::dynamic_pointer_cast<RtcProducer>(shared_from_this());
			RtcService::GetInstance().UnregisterRtcInterface(slef);
			
			// 清空Session的Producer
			GetSession()->SetProducer(nullptr);
		});
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief DTLS连接失败回调（On DTLS Failed）
	*  
	*  该方法在DTLS握手失败时被调用。它会清理所有资源，
	*  并从RTC服务中注销RTC接口。
	*  
	*  处理流程：
	*  1. 记录警告日志，标记DTLS连接失败
	*  2. 设置dtls_done_标志为false
	*  3. 获取会话名称
	*  4. 在工作线程中执行清理操作：
	*     a. 从RTC服务中注销RTC接口
	*     b. 清空Session的Producer
	*  
	*  DTLS失败原因：
	*  - 证书验证失败
	*  - 握手超时
	*  - 网络错误
	*  - 协议不匹配
	*  
	*  @param dtls 指向DTLS对象的指针
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 清理操作在工作线程中执行，避免阻塞网络线程
	*  @note DTLS握手失败后，无法建立安全连接
	*/
	void RtcProducer::OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls *dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS failed";
		dtls_done_ = false;

		// 获取会话名称
		std::string session_name = GetSession()->SessionName();
		
		// 在工作线程中执行清理操作
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			// 从RTC服务中注销RTC接口
			std::shared_ptr<RtcProducer> slef = std::dynamic_pointer_cast<RtcProducer>(shared_from_this());
			RtcService::GetInstance().UnregisterRtcInterface(slef);
			
			// 清空Session的Producer
			GetSession()->SetProducer(nullptr);
			
		});
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief DTLS应用数据接收回调（On DTLS Application Data Received）
	*  
	*  该方法在DTLS连接上接收到应用数据时被调用。对于WebRTC，应用数据通常是SCTP数据包，
	*  用于传输DataChannel数据。
	*  
	*  处理流程：
	*  1. 记录警告日志，标记接收到DTLS应用数据
	*  2. 如果SCTP关联存在，将数据传递给SCTP处理
	*  3. SCTP会解析数据包，提取DataChannel消息
	*  
	*  SCTP协议说明：
	*  - SCTP（Stream Control Transmission Protocol）是一种传输层协议
	*  - WebRTC使用SCTP over DTLS传输DataChannel数据
	*  - SCTP支持多流、有序/无序传输、可靠/不可靠传输
	*  
	*  @param dtls 指向DTLS对象的指针
	*  @param data 应用数据的指针
	*  @param len 应用数据的长度
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 如果SCTP关联不存在，数据将被丢弃
	*  @note 应用数据通常是SCTP数据包
	*/
	void RtcProducer::OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls *dtls, const uint8_t* data, size_t len)
	{
		// 记录日志，标记接收到DTLS应用数据
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS application data recice data ";
		
		// 如果SCTP关联存在，将数据传递给SCTP处理
		if (sctp_)
		{
			sctp_->ProcessSctpData(data, len);
		}
	}


  


}