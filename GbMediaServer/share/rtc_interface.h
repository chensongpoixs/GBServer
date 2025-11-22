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
				   date:  2025-11-03

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


#ifndef _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_
#define _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
#include "consumer/consumer.h"
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h"

#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"



#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/muxer/muxer.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "share/share_resource.h"
#include "libmedia_transfer_protocol/librtcp/twcc_context.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/nack.h"
#include <memory>
namespace gb_media_server {
 
	/**
	*  @author chensong
	*  @date 2025-11-03
	*  @brief RTC接口基类（RTC Interface）
	*  
	*  RtcInterface是GBMediaServer流媒体服务器中所有RTC相关类的基类接口。
	*  它提供了WebRTC协议的核心功能，包括SDP处理、DTLS握手、SRTP加密、
	*  SCTP数据通道等。RtcProducer和RtcConsumer都继承自此类。
	*  
	*  WebRTC协议说明：
	*  - WebRTC（Web Real-Time Communication）是一种实时通信技术
	*  - WebRTC使用SDP（Session Description Protocol）进行信令交换
	*  - WebRTC使用DTLS（Datagram Transport Layer Security）进行密钥交换
	*  - WebRTC使用SRTP（Secure Real-time Transport Protocol）进行媒体加密传输
	*  - WebRTC使用SCTP（Stream Control Transmission Protocol）进行数据通道传输
	*  
	*  RtcInterface功能：
	*  1. SDP处理：解析Offer和生成Answer
	*  2. DTLS握手：建立安全连接，交换密钥
	*  3. SRTP加密：加密和解密RTP/RTCP包
	*  4. SCTP数据通道：支持非媒体数据传输
	*  5. RTP/RTCP处理：发送和接收媒体包
	*  6. TWCC（Transport-CC）：带宽估计和拥塞控制
	*  
	*  @note RtcInterface是抽象基类，不能直接实例化
	*  @note 子类必须实现所有纯虚函数
	*  @note 提供了完整的WebRTC协议栈实现
	*  
	*  继承层次：
	*  - RtcProducer: 继承自RtcInterface，用于接收WebRTC推流
	*  - RtcConsumer: 继承自RtcInterface，用于发送WebRTC拉流
	*  
	*  使用示例：
	*  @code
	*  // RtcInterface不能直接实例化，通过子类使用
	*  auto producer = std::make_shared<RtcProducer>(stream, session);
	*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
	*  @endcode
	*/
	class RtcInterface  : public libmedia_transfer_protocol::librtc::SctpAssociation::Listener
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化RtcInterface实例。它会初始化所有成员变量，
		*  包括SDP对象、DTLS对象、SRTP会话、SCTP关联等。
		*  
		*  初始化流程：
		*  1. 初始化本地用户名片段（ufrag）和密码（pwd）
		*  2. 初始化SDP对象
		*  3. 初始化DTLS对象，连接DTLS信号
		*  4. 初始化SRTP发送和接收会话（初始为空）
		*  5. 初始化RTP序列号和RTX序列号
		*  6. 初始化RTP头部和扩展管理器
		*  7. 初始化TWCC上下文
		*  8. 初始化SCTP关联
		*  
		*  @note 构造函数会生成随机的用户名片段和密码
		*  @note DTLS对象会在SDP处理时进一步配置
		*  @note SRTP会话会在DTLS握手成功后创建
		*  
		*  使用示例：
		*  @code
		*  // 通过子类构造函数隐式调用
		*  auto producer = std::make_shared<RtcProducer>(stream, session);
		*  @endcode
		*/
		explicit RtcInterface();

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtcInterface实例。它会释放所有相关资源，
		*  包括DTLS对象、SRTP会话、SCTP关联等。
		*  
		*  清理流程：
		*  1. 关闭DTLS连接（如果已连接）
		*  2. 释放SRTP发送和接收会话
		*  3. 关闭SCTP关联
		*  4. 清理所有资源
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual ~RtcInterface() ;

	public:
	   virtual 	bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp) = 0;
	   virtual const std::string& LocalUFrag() const = 0;
	   virtual const std::string& LocalPasswd() const = 0;
	   virtual const std::string& RemoteUFrag() const = 0;
	   virtual std::string BuildAnswerSdp() = 0;

		//开始DTLS选择客户端还是服务端挥手交换 
	   virtual void MayRunDtls() = 0;
	public:

		bool SendSrtpRtp(uint8_t* data, size_t  size);
		bool SendSrtpRtcp(uint8_t* data, size_t size);


		void SendDatachannel(uint16_t streamId, uint32_t ppid, const char* msg, size_t len);
		// 

		virtual void OnDtlsRecv(const uint8_t* buf, size_t size) = 0;
		virtual void OnSrtpRtp(  uint8_t* data, size_t size) = 0;
		virtual void OnSrtpRtcp(  uint8_t* data, size_t size) = 0;
	
		
	public:
		virtual void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		virtual void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert) = 0;
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len) = 0;

		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len) = 0;
	public:
		void onSendTwcc(uint32_t ssrc, const std::string& twcc_fci);




		void CreateDataChannel();


		void AddVideoPacket(std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend> rtp_packet);


		void RequestNack(const libmedia_transfer_protocol::rtcp::Nack& nack);
	public:

		// sctp inferface
		virtual void OnSctpAssociationConnecting(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationConnected(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationFailed(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationClosed(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationSendData(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation, 
			const uint8_t* data, size_t len)  override;
		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;

	public:
		// rtc 特别增加的接口
		virtual  const rtc::SocketAddress& RtcRemoteAddress() const
		{
			return rtc_remote_address_;
		}
		virtual void  SetRtcRemoteAddress(const rtc::SocketAddress& addr);

	

	public:
		static std::string GetUFrag(int size);
		static uint32_t GetSsrc(int size);
	protected:


		std::string local_ufrag_;
		std::string local_passwd_;  //[12, 32]
		libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		//Dtls dtls_;

		//libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		libmedia_transfer_protocol::libssl::Dtls   dtls_;

		bool dtls_done_{ false };

		rtc::SocketAddress             rtc_remote_address_;
		libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_send_session_;
		libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_recv_session_;




		uint32_t      audio_seq_ = 100;
		uint32_t      video_seq_ = 100;
		uint32_t      video_rtx_seq_ = 100;
		//libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;
		libmedia_transfer_protocol::RTPHeader  rtp_header_;
		libmedia_transfer_protocol::RtpHeaderExtensionMap    extension_manager_;
		libmedia_transfer_protocol::librtcp::TwccContext     twcc_context_;

		//libmedia_transfer_protocol::librtc::SctpAssociationImp::Ptr   sctp_;
		std::shared_ptr< libmedia_transfer_protocol::librtc::SctpAssociationImp> sctp_;


		std::unordered_map<uint32_t, std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend>>   rtp_video_packets_;
	};
 
}
#endif // _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_