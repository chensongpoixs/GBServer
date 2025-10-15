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
namespace gb_media_server {
	class Connection;
	class Stream;
	class Session;
	class PlayRtcUser : public  PlayerUser  , public sigslot::has_slots<>
	{
	public:
		explicit PlayRtcUser( std::shared_ptr<Connection> &ptr,  std::shared_ptr<Stream> &stream,   std::shared_ptr<Session> &s);
		virtual ~PlayRtcUser();


	public:
		bool ProcessOfferSdp(const std::string &sdp);
		const std::string &LocalUFrag() const;
		const std::string &LocalPasswd() const;
		const std::string &RemoteUFrag() const;
		std::string BuildAnswerSdp();
	public:


		void SetRemoteSocketAddress(const rtc::SocketAddress & addr);
		void OnDtlsRecv(const char *buf, size_t size);

	public:
		//sigslot::signal3<const char *, size_t, Dtls*>
		void OnDtlsSendPakcet(const char *data, size_t len, libmedia_transfer_protocol::librtc::Dtls* dtls);
		void OnDtlsHandshakeDone(libmedia_transfer_protocol::librtc::Dtls *dtls);
		//sigslot::signal1<  Dtls*>
		//	SignalDtlsHandshakeDone;
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
		libmedia_transfer_protocol::librtc::Dtls   dtls_;

		bool dtls_done_{ false };

		rtc::SocketAddress             remote_address_;
	};
}

#endif // _C_WEBRTC_PLAY_H_