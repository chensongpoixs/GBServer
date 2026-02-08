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
#include "gb_media_server_log.h"
namespace gb_media_server
{
	/**
	*  @brief DTLS连接中回调（DTLS Connecting）
	*  
	*  当DTLS握手开始时触发该回调。此时DTLS连接正在建立中，
	*  但尚未完成密钥交换和证书验证。
	*  
	*  @param dtls DTLS对象指针
	*  @note 该回调用于记录DTLS连接状态，便于调试和监控
	*/
	 void RtcConsumer::OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO) << "DTLS connecting" ;
	}
	
	/**
	*  @brief DTLS连接成功回调（DTLS Connected）
	*  
	*  当DTLS握手成功完成时触发该回调。此时已经完成密钥交换和证书验证，
	*  可以开始创建SRTP会话并发送加密的媒体数据。
	*  
	*  处理流程：
	*  1. 记录DTLS连接成功日志
	*  2. 删除旧的SRTP会话（如果存在）
	*  3. 创建SRTP发送会话（OUTBOUND），使用本地密钥
	*  4. 创建SRTP接收会话（INBOUND），使用远程密钥
	*  5. 设置dtls_done_标志为true，表示可以开始发送媒体数据
	*  6. 如果SDP中包含数据通道参数，创建SCTP关联
	*  
	*  SRTP会话说明：
	*  - SRTP（Secure Real-time Transport Protocol）用于加密RTP/RTCP包
	*  - 发送会话使用本地密钥加密发送的RTP/RTCP包
	*  - 接收会话使用远程密钥解密接收的RTP/RTCP包
	*  - 密钥通过DTLS握手协商得到
	*  
	*  @param dtls DTLS对象指针
	*  @param srtpCryptoSuite SRTP加密套件（例如AES_CM_128_HMAC_SHA1_80）
	*  @param srtpLocalKey 本地SRTP密钥，用于加密发送的数据
	*  @param srtpLocalKeyLen 本地密钥长度
	*  @param srtpRemoteKey 远程SRTP密钥，用于解密接收的数据
	*  @param srtpRemoteKeyLen 远程密钥长度
	*  @param remote_cert 远程证书（用于验证对端身份）
	*  @note 如果SRTP会话创建失败，会记录错误日志
	*  @note SCTP关联用于WebRTC数据通道，如果不需要数据通道可以不创建
	*/
	void RtcConsumer::OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
		libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
		uint8_t* srtpLocalKey,
		size_t srtpLocalKeyLen,
		uint8_t* srtpRemoteKey,
		size_t srtpRemoteKeyLen,
		std::string& remote_cert)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "DTLS connected";



		delete srtp_send_session_;
		srtp_send_session_ = nullptr;
		delete srtp_recv_session_;
		srtp_recv_session_ = nullptr;


		try
		{
			srtp_send_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::OUTBOUND, srtpCryptoSuite, srtpLocalKey, srtpLocalKeyLen);
		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR)<< "error creating SRTP sending session: " <<  error.what();
		}
		try
		{
			srtp_recv_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::INBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);
 
		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR) << "error creating SRTP receiving session: %s", error.what();

			delete srtp_send_session_;
			srtp_send_session_ = nullptr;
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "dtls handshake done.";
		dtls_done_ = true;
		//srtp_session_.Init(dtls_.RecvKey(), dtls_.SendKey());
		// return;
		 // 完成验证后进行发送
		if (sdp_.GetDataChannelParams().application)
		{
			CreateDataChannel();
		}
		
		//StartCapture();
	}
	
	/**
	*  @brief DTLS发送数据包回调（DTLS Send Packet）
	*  
	*  当DTLS需要发送数据包时触发该回调。DTLS握手过程中会生成多个握手消息，
	*  这些消息需要通过UDP发送给对端。
	*  
	*  处理流程：
	*  1. 记录DTLS发送数据包日志
	*  2. 将数据包封装为rtc::Buffer
	*  3. 通过RTC服务器发送数据包到远程地址
	*  
	*  @param dtls DTLS对象指针
	*  @param data 待发送的数据指针
	*  @param len 数据长度
	*  @note 该方法会在DTLS握手过程中被多次调用
	*  @note 数据包通过UDP发送，不保证可靠传输，DTLS会处理重传
	*/
	void RtcConsumer::OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t *data, size_t len)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "dtls send size:" << len;
		 
		rtc::Buffer buffer(data, len);
		GbMediaService::GetInstance().GetRtcServer()->SendPacketTo(std::move(buffer), rtc_remote_address_, rtc::PacketOptions());
	}
	
	/**
	*  @brief DTLS连接关闭回调（DTLS Closed）
	*  
	*  当DTLS连接被远程端关闭时触发该回调。此时需要清理相关资源，
	*  并从会话中移除该消费者。
	*  
	*  处理流程：
	*  1. 记录DTLS连接关闭日志
	*  2. 设置dtls_done_标志为false，停止发送媒体数据
	*  3. 获取会话名称
	*  4. 在工作线程中执行清理操作：
	*     - 从RTC服务中注销RTC接口
	*     - 从会话中移除该消费者
	*  
	*  @param dtls DTLS对象指针
	*  @note 该方法会在工作线程中异步执行清理操作
	*  @note 清理操作包括注销RTC接口和移除消费者
	*/
	void RtcConsumer::OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls *dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS remotely closed";
		dtls_done_ = false;

		//StopCapture();

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().UnregisterRtcInterface(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);
		});
		// 
	}
	
	/**
	*  @brief DTLS连接失败回调（DTLS Failed）
	*  
	*  当DTLS握手失败时触发该回调。失败原因可能包括证书验证失败、
	*  超时、网络错误等。此时需要清理相关资源，并从会话中移除该消费者。
	*  
	*  处理流程：
	*  1. 记录DTLS连接失败日志
	*  2. 设置dtls_done_标志为false，停止发送媒体数据
	*  3. 获取会话名称
	*  4. 在工作线程中执行清理操作：
	*     - 从RTC服务中注销RTC接口
	*     - 从会话中移除该消费者
	*  
	*  @param dtls DTLS对象指针
	*  @note 该方法会在工作线程中异步执行清理操作
	*  @note DTLS失败通常意味着连接无法建立，需要重新连接
	*/
	void RtcConsumer::OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls *dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS failed";
		dtls_done_ = false;

		//StopCapture();

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().UnregisterRtcInterface(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);
			
		});
		// 
	}
	
	/**
	*  @brief DTLS应用数据接收回调（DTLS Application Data Received）
	*  
	*  当DTLS连接建立后，接收到应用层数据时触发该回调。
	*  对于WebRTC，应用层数据通常是SCTP数据包，用于数据通道传输。
	*  
	*  处理流程：
	*  1. 记录接收到应用数据的日志
	*  2. 如果SCTP关联存在，将数据传递给SCTP处理
	*  3. SCTP会解析数据包并触发相应的数据通道回调
	*  
	*  SCTP说明：
	*  - SCTP（Stream Control Transmission Protocol）用于WebRTC数据通道
	*  - SCTP数据包通过DTLS连接传输，提供可靠的数据传输
	*  - 数据通道可以传输任意二进制数据或文本数据
	*  
	*  @param dtls DTLS对象指针
	*  @param data 接收到的应用数据指针
	*  @param len 数据长度
	*  @note 如果SCTP关联不存在，数据会被忽略
	*  @note 该回调只在DTLS连接建立后才会触发
	*/
	void RtcConsumer::OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls *dtls, const uint8_t* data, size_t len)
	{
		// Pass it to the parent transport.
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS application data recice data ";
		if (sctp_)
		{
			sctp_->ProcessSctpData(data, len);
		}
	}


  


}