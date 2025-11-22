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
				   date:  2025-10-18

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


#ifndef _C_GB_MEDIA_SERVER_RTC_Producer_H_
#define _C_GB_MEDIA_SERVER_RTC_Producer_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
 
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h" 
#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 


#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/libmpeg/mpeg_decoder.h"
#include "producer/producer.h"
#include "share/rtc_interface.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_context_recv.h"

#include "libmedia_codec/video_codecs/nal_parse_factory.h"

namespace gb_media_server {


	class RtcProducer : public RtcInterface,   public Producer, public sigslot::has_slots<>
	{
	public:
		explicit RtcProducer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		virtual ~RtcProducer();
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer)  ;




		virtual void RequestKeyFrame();
		virtual  void SetStreamStatus(bool status);
		void OnTimer();
	public:
		virtual ShareResourceType ShareResouceType() const { return kProducerTypeRtc; }
	public:
		virtual bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp);
		virtual const std::string& LocalUFrag() const;
		virtual const std::string& LocalPasswd() const;
		virtual const std::string& RemoteUFrag() const;
		virtual std::string BuildAnswerSdp();

		//开始DTLS选择客户端还是服务端挥手交换 
		virtual void MayRunDtls();
	public:



		virtual void OnDtlsRecv(const uint8_t* buf, size_t size);
		virtual void OnSrtpRtp(  uint8_t* data, size_t size);
		virtual void OnSrtpRtcp(  uint8_t* data, size_t size);
	public:
		//virtual void OnVideoFrame(const libmedia_codec::EncodedImage& frame);
		//virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);

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
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len);

		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls);
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls);
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len);

	public:

		virtual  void OnDataChannel(
			const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params,
			uint32_t ppid, const uint8_t* msg, size_t len);

		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;
	public:
		//virtual ShareResourceType ShareResouceType() const { return kConsumerTypeRTC; }

	private:
	//	static std::string GetUFrag(int size);
	//	static uint32_t GetSsrc(int size);
	private:
		//std::string local_ufrag_;
		//std::string local_passwd_;  //[12, 32]
		//libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		////Dtls dtls_;
		//
		////libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		//libmedia_transfer_protocol::libssl::Dtls   dtls_;
		//
		//bool dtls_done_{ false };
		//
		////rtc::SocketAddress             remote_address_;
		//libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_send_session_;
		//libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_recv_session_;
		//
		//
		//
		//
		//uint32_t      audio_seq_ = 100;
		//uint32_t      video_seq_ = 100;
		//libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;



	private:


		//std::unique_ptr< libmedia_transfer_protocol::libmpeg::MpegDecoder>    mpeg_decoder_;

		webrtc::ScopedTaskSafety task_safety_;
		//rtc::Buffer recv_buffer_;
		   uint8_t  * recv_buffer_;
		int32_t recv_buffer_size_;


		std::unique_ptr<libmedia_codec::NalParseInterface>  nal_parse_;

		std::unique_ptr<libmedia_transfer_protocol::librtcp::RtcpContextRecv>   rtcp_context_recv_;
		int32_t   num_skipped_packets_ = 0;


		int64_t    rtcp_rr_timestamp_;



		int64_t    request_key_frame_;
		


		bool       stream_status_ = true;
	};
}


#endif // 