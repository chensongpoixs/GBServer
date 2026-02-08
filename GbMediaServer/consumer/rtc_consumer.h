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


#ifndef _C_GB_MEDIA_SERVER_RTC_PLAY_USER_H_
#define _C_GB_MEDIA_SERVER_RTC_PLAY_USER_H_

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
#include "consumer/rtc_consumer.h"
#include "share/rtc_interface.h"
namespace gb_media_server {
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief RTC消费者类（RTC Consumer）
	*  
	*  RtcConsumer是GBMediaServer流媒体服务器中用于WebRTC协议播放的消费者类。
	*  它继承自RtcInterface和Consumer基类，负责将服务器中的媒体流封装为RTP/RTCP包
	*  并通过WebRTC协议发送给客户端。
	*  
	*  WebRTC协议说明：
	*  - WebRTC（Web Real-Time Communication）是一种实时通信技术，支持低延迟的
	*    音视频传输和数据通道
	*  - WebRTC使用SDP（Session Description Protocol）进行信令交换
	*  - WebRTC使用DTLS（Datagram Transport Layer Security）进行密钥交换
	*  - WebRTC使用SRTP（Secure Real-time Transport Protocol）进行媒体加密传输
	*  - WebRTC使用SCTP（Stream Control Transmission Protocol）进行数据通道传输
	*  
	*  RtcConsumer功能：
	*  1. 接收来自Stream的视频帧和音频帧
	*  2. 将视频帧和音频帧封装为RTP包并加密发送
	*  3. 处理RTCP控制包（接收报告、发送报告、反馈等）
	*  4. 支持DTLS握手和SRTP加密
	*  5. 支持SCTP数据通道
	*  6. 支持关键帧请求（PLI/FIR）
	*  
	*  工作流程：
	*  1. 客户端通过HTTP API发送SDP Offer
	*  2. RtcConsumer处理SDP Offer并生成SDP Answer
	*  3. 开始DTLS握手，交换证书和密钥
	*  4. DTLS握手成功后，建立SRTP会话
	*  5. 建立SCTP关联，支持数据通道
	*  6. 开始发送RTP媒体包和RTCP控制包
	*  
	*  RTP/RTCP说明：
	*  - RTP（Real-time Transport Protocol）用于传输音视频数据
	*  - RTCP（RTP Control Protocol）用于传输控制信息（丢包统计、带宽估计等）
	*  - RTP包包含序列号、时间戳、SSRC等头部信息
	*  - RTCP包用于反馈接收质量、请求关键帧等
	*  
	*  @note RtcConsumer使用RtcInterface基类提供的DTLS、SRTP和SCTP功能
	*  @note 视频帧会被封装为H264格式的RTP包
	*  @note 音频帧会被封装为OPUS或PCMU格式的RTP包
	*  @note 支持RTX重传机制，提高传输可靠性
	*  
	*  使用示例：
	*  @code
	*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
	*  session->AddConsumer(consumer);
	*  @endcode
	*/
	class RtcConsumer : public  RtcInterface,  public  Consumer // ,
//#if TEST_RTC_PLAY
//		public  libmedia_codec::EncodeImageObser, public libmedia_codec::EncodeAudioObser,
//#endif // 
	//	public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-13
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于创建RtcConsumer实例。它会初始化流对象、会话对象、
		*  SDP对象、DTLS对象等，并设置音频和视频的SSRC。
		*  
		*  初始化流程：
		*  1. 调用基类RtcInterface和Consumer的构造函数
		*  2. 生成音频和视频的SSRC（Synchronization Source Identifier）
		*  3. 初始化SDP对象，设置音频/视频SSRC和RTX SSRC
		*  4. 初始化DTLS对象，连接DTLS信号
		*  5. 设置SDP的本地指纹、服务器地址和端口
		*  6. 设置流名称
		*  
		*  SSRC说明：
		*  - SSRC用于标识RTP流的源，每个媒体流有唯一的SSRC
		*  - 音频SSRC和视频SSRC通常是连续的
		*  - RTX SSRC用于重传机制，通常是视频SSRC+1
		*  
		*  @param stream 流对象引用，用于获取媒体流，不能为空
		*  @param s 会话对象引用，用于管理会话状态，不能为空
		*  @note 流对象和会话对象通过引用传递，确保生命周期管理正确
		*  @note DTLS信号连接会在构造函数中完成
		*  
		*  使用示例：
		*  @code
		*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
		*  @endcode
		*/
		explicit RtcConsumer(   std::shared_ptr<Stream> &stream, const  std::shared_ptr<Session> &s);

		/**
		*  @author chensong
		*  @date 2025-10-13
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtcConsumer实例。它会释放所有相关资源，
		*  包括DTLS对象、SRTP会话、SCTP关联等。
		*  
		*  清理流程：
		*  1. 断开DTLS连接（如果已连接）
		*  2. 关闭SRTP会话
		*  3. 关闭SCTP关联
		*  4. 调用基类的析构函数进行基类资源清理
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*  @note 流对象和会话对象由外部管理，不需要在此释放
		*/
		virtual ~RtcConsumer();


		

		void RequestKeyFrame();
	
	public:
		virtual  bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string &sdp);
		virtual const std::string &LocalUFrag() const;
		virtual const std::string &LocalPasswd() const;
		virtual const std::string &RemoteUFrag() const;
		virtual std::string BuildAnswerSdp();

		//开始DTLS选择客户端还是服务端挥手交换 
		virtual void MayRunDtls();
	public:


		 
		virtual void OnDtlsRecv(const uint8_t *buf, size_t size);
		virtual void OnSrtpRtp(  uint8_t * data, size_t size);
		virtual void OnSrtpRtcp(  uint8_t * data, size_t size);
	public:
		virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame);
		virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);

	public:

	
	public:
		virtual void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls);
		virtual void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert);
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t *data, size_t len);
		 
		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls *dtls);
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls *dtls);
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls *dtls, const uint8_t* data, size_t len);
		 
	public:


	public:
		virtual void OnDataChannel(const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, uint32_t ppid, const uint8_t* msg, size_t len);
		
		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;
		virtual ShareResourceType ShareResouceType() const   { return kConsumerTypeRTC; }
		 
	private:
		//static std::string GetUFrag(int size);
		//static uint32_t GetSsrc(int size);
	private:
		// std::string local_ufrag_;
		// std::string local_passwd_;  //[12, 32]
		// libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		// //Dtls dtls_;
		// 
		// //libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		// libmedia_transfer_protocol::libssl::Dtls   dtls_;
		// 
		// bool dtls_done_{ false };
		// 
		// //rtc::SocketAddress             remote_address_;
		// libmedia_transfer_protocol::libsrtp::SrtpSession*   srtp_send_session_;
		// libmedia_transfer_protocol::libsrtp::SrtpSession *  srtp_recv_session_;
		// 
		// 
		// 
		// 
		// uint32_t      audio_seq_ = 100;
		// uint32_t      video_seq_ = 100;
		//  libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;

		
		

	};
}

#endif // _C_WEBRTC_PLAY_H_