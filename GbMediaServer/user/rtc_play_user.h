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



 ******************************************************************************/


#ifndef _C_GB_MEDIA_SERVER_RTC_PLAY_USER_H_
#define _C_GB_MEDIA_SERVER_RTC_PLAY_USER_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
#include "user/user.h"
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h"
#include "user/player_user.h"
#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 


#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
namespace gb_media_server {
	//class Connection;
	class Stream;
	class Session;
	class PlayRtcUser : public  PlayerUser  ,
#if TEST_RTC_PLAY
		public  libmedia_codec::EncodeImageObser,
#endif // 
		public sigslot::has_slots<>
	{
	public:
		explicit PlayRtcUser( std::shared_ptr<Connection> &ptr,  std::shared_ptr<Stream> &stream,   std::shared_ptr<Session> &s);
		virtual ~PlayRtcUser();

	public:
		void SetCapture(bool value);
	public:
		bool ProcessOfferSdp(const std::string &sdp);
		const std::string &LocalUFrag() const;
		const std::string &LocalPasswd() const;
		const std::string &RemoteUFrag() const;
		std::string BuildAnswerSdp();

		void MayRunDtls();
	public:


		void SetRemoteSocketAddress(const rtc::SocketAddress & addr);
		void OnDtlsRecv(const char *buf, size_t size);
		void OnSrtpRtp(const uint8_t * data, size_t size);
		void OnSrtpRtcp(const uint8_t * data, size_t size);
	public:
		virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame);
	public:
		/*
		sigslot::signal1<Dtls*>     SignalDtlsConnecting;
			sigslot::signal7<Dtls*, libsrtp::CryptoSuite  ,
				uint8_t*  ,
				size_t  ,
				uint8_t*  ,
				size_t  ,
				std::string&  >     SignalDtlsConnected;

			sigslot::signal1<Dtls*>     SignalDtlsClose;
			sigslot::signal1<Dtls*>     SignalDtlsFailed;

			sigslot::signal3<const uint8_t *, size_t, Dtls*>
				SignalDtlsSendPakcet;
			//sigslot::signal1<  Dtls*>
			//	SignalDtlsHandshakeDone; 
			sigslot::signal3<  Dtls*, const uint8_t *, size_t>
				SignalDtlsApplicationDataReceived;
		*/
		//sigslot::signal3<const char *, size_t, Dtls*>
		void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls);
		void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert);
		void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t *data, size_t len);
		//void OnDtlsHandshakeDone(libmedia_transfer_protocol::libssl::Dtls *dtls);
		void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls *dtls);
		void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls *dtls);
		void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls *dtls, const uint8_t* data, size_t len);
		//sigslot::signal1<  Dtls*>
		//	SignalDtlsHandshakeDone;

	public:

#if TEST_RTC_PLAY
		virtual void   SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> f) override;

#endif //

	public:

		virtual   UserType   GetUserType() const  override;
	private:
		static std::string GetUFrag(int size);
		static uint32_t GetSsrc(int size);
	private:
		std::string local_ufrag_;
		std::string local_passwd_;  //[12, 32]
		libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		//Dtls dtls_;

		//libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		libmedia_transfer_protocol::libssl::Dtls   dtls_;

		bool dtls_done_{ false };

		rtc::SocketAddress             remote_address_;
		libmedia_transfer_protocol::libsrtp::SrtpSession*   srtp_send_session_;
		libmedia_transfer_protocol::libsrtp::SrtpSession *  srtp_recv_session_;


#if TEST_RTC_PLAY
		std::unique_ptr< rtc::Thread>        video_encoder_thread_;
		std::unique_ptr< libmedia_codec::X264Encoder>                          x264_encoder_;
		rtc::scoped_refptr<libcross_platform_collection_render::CapturerTrackSource>     capturer_track_source_;
#endif //
		uint32_t      video_seq_ = 100;
		 libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;

		 rtc::CopyOnWriteBuffer                     sps_;
		 rtc::CopyOnWriteBuffer						pps_;
		 bool										capture_type_;//采集桌面画面播放
	};
}

#endif // _C_WEBRTC_PLAY_H_