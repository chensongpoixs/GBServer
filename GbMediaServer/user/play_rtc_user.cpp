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
#include "user/play_rtc_user.h"
#include <random>
#include "user/user.h"
namespace gb_media_server
{
	PlayRtcUser::PlayRtcUser(  std::shared_ptr<Connection> &ptr,   std::shared_ptr<Stream> &stream,   std::shared_ptr<Session> &s)
	:PlayerUser(ptr, stream, s)
	{
	
		local_ufrag_ = GetUFrag(8);
		local_passwd_ = GetUFrag(32);
		uint32_t audio_ssrc = GetSsrc(10);
		uint32_t video_ssrc = audio_ssrc + 1;

		sdp_.SetLocalUFrag(local_ufrag_);
		sdp_.SetLocalPasswd(local_passwd_);
		sdp_.SetAudioSsrc(audio_ssrc);
		sdp_.SetVideoSsrc(video_ssrc);
		dtls_certs_.Init();
		sdp_.SetFingerprint(dtls_certs_.Fingerprint());
		// 本地ip port 
		sdp_.SetServerAddr("192.168.1.2");
		sdp_.SetServerPort(9001);
		sdp_.SetStreamName("23423432432"/*s->SessionName()*/);
	}
	PlayRtcUser:: ~PlayRtcUser(){}

 
	bool PlayRtcUser::ProcessOfferSdp(const std::string &sdp) {
		return sdp_.Decode(sdp);
	}
	const std::string &PlayRtcUser::LocalUFrag() const {
		return sdp_.GetLocalUFrag();
	}
	const std::string &PlayRtcUser::LocalPasswd() const {
		return sdp_.GetLocalPasswd();
	}
	const std::string &PlayRtcUser::RemoteUFrag() const {
		return sdp_.GetRemoteUFrag();
	}
	std::string PlayRtcUser::BuildAnswerSdp() {
		sdp_.SetFingerprint(dtls_certs_.Fingerprint());
		return sdp_.Encode();
	}

 
	  std::string PlayRtcUser::GetUFrag(int size) {
		  static std::string table = "1234567890abcdefgihjklmnopqrstuvwsyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		  std::string frag;

		  static std::mt19937 mt{ std::random_device{}() };
		  static std::uniform_int_distribution<> rand(0, table.size());

		  frag.resize(size);
		  for (int i = 0; i < size; i++)
		  {
			  frag[i] = table[(rand(mt) % table.size())];
		  }

		  return frag;
	  }
	  uint32_t PlayRtcUser::GetSsrc(int size)
	  {
		  static std::mt19937 mt{ std::random_device{}() };
		  static std::uniform_int_distribution<> rand(10000000, 99999999);

		  return rand(mt);
	  }


	  UserType   PlayRtcUser::GetUserType() const
	  {
		  return UserType::kUserTypePlayerWebRTC;
	  }
}