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
#include "server/session.h"

#include "rtc_base/logging.h"
#include "rtc_base/buffer.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format_h264.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include <vector>
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format.h"
#include <memory>
#include "server/rtc_service.h"

namespace gb_media_server
{
	PlayRtcUser::PlayRtcUser(  std::shared_ptr<Connection> &ptr,   std::shared_ptr<Stream> &stream,   std::shared_ptr<Session> &s)
	:PlayerUser(ptr, stream, s)
		, rtp_header_extension_map_()
		, capture_type_(false)
	{
	
		local_ufrag_ = GetUFrag(8);
		local_passwd_ = GetUFrag(32);
		uint32_t audio_ssrc = GetSsrc(10);
		uint32_t video_ssrc = audio_ssrc + 1;

		sdp_.SetLocalUFrag(local_ufrag_);
		sdp_.SetLocalPasswd(local_passwd_);
		sdp_.SetAudioSsrc(audio_ssrc);
		sdp_.SetVideoSsrc(video_ssrc);
		//dtls_certs_.Init();
		dtls_.Init();
		dtls_.SignalDtlsSendPakcet.connect(this, &PlayRtcUser::OnDtlsSendPakcet);
		dtls_.SignalDtlsHandshakeDone.connect(this, &PlayRtcUser::OnDtlsHandshakeDone);
		dtls_.SignalDtlsClose.connect(this, &PlayRtcUser::OnDtlsClosed);
		sdp_.SetFingerprint(dtls_.Fingerprint());
		// 本地ip port 
		sdp_.SetServerAddr("192.168.1.2");
		sdp_.SetServerPort(9001);
		sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);
		//rtp_header_extension_map_.Register<libmedia_transfer_protocol::TransportSequenceNumber>(libmedia_transfer_protocol::kRtpExtensionTransportSequenceNumber);
	}
	PlayRtcUser:: ~PlayRtcUser(){
		GBMEDIASERVER_LOG_T_F(LS_INFO);
#if TEST_RTC_PLAY
		{
			if (capture_type_)
				if (video_encoder_thread_)
				{
					video_encoder_thread_->Stop();
				}
			if (x264_encoder_)
			{
				//	x264_encoder_->SetSendFrame(nullptr);
				x264_encoder_->Stop();
			}
			if (capturer_track_source_)
			{
				//	capturer_track_source_->set_catprue_callback(nullptr, nullptr);
				capturer_track_source_->Stop();
			}
		}
#endif // 	
		dtls_.SignalDtlsSendPakcet.disconnect(this);
		dtls_.SignalDtlsHandshakeDone.disconnect(this);
		dtls_.SignalDtlsClose.disconnect(this);
		
		
		

		
		
	}

 
	void PlayRtcUser::SetCapture(bool value)
	{
		capture_type_ = value;
	}

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
		sdp_.SetFingerprint(dtls_.Fingerprint());
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


	  void PlayRtcUser::SetRemoteSocketAddress(const rtc::SocketAddress & addr)
	  {
		  remote_address_ = addr;
	  }
	  void PlayRtcUser::OnDtlsRecv(const char *buf, size_t size)
	  {
		  dtls_.OnRecv(buf, size);
	  }
	  void PlayRtcUser::OnDtlsSendPakcet(const char * data, size_t size, libmedia_transfer_protocol::librtc::Dtls * dtls)
	  {
		  GBMEDIASERVER_LOG(LS_INFO) << "dtls send size:" << size;
		  //PacketPtr packet = Packet::NewPacket(size);
		  //memcpy(packet->Data(), data, size);
		  //packet->SetPacketSize(size);
		  //
		  //packet->SetExt(addr_);
		  //auto server = sLiveService->GetWebrtcServer();
		  //server->SendPacket(packet);

		  rtc::Buffer buffer(data, size);
		  GbMediaService::GetInstance().GetRtcServer()->SendPacketTo(buffer, remote_address_, rtc::PacketOptions());

	  }

	  void PlayRtcUser::OnDtlsHandshakeDone(libmedia_transfer_protocol::librtc::Dtls * dtls)
	  {
		  GBMEDIASERVER_LOG(LS_INFO) << "dtls handshake done.";
		  dtls_done_ = true;
		  srtp_session_.Init(dtls_.RecvKey(), dtls_.SendKey());
		 // return;
		  // 完成验证后进行发送

#if TEST_RTC_PLAY
		  if (capture_type_)
		  {
			  x264_encoder_ = std::make_unique<libmedia_codec::X264Encoder>();
			  x264_encoder_->SetSendFrame(this);
			  x264_encoder_->Start();
			  video_encoder_thread_ = rtc::Thread::Create();
			  video_encoder_thread_->SetName("video_encoder_thread", NULL);
			  video_encoder_thread_->Start();

			  capturer_track_source_ = libcross_platform_collection_render::CapturerTrackSource::Create(false);
			  capturer_track_source_->set_catprue_callback(x264_encoder_.get(), video_encoder_thread_.get());
			  capturer_track_source_->StartCapture();
		  }
		
#endif // 1
	  }

	  void PlayRtcUser::OnDtlsClosed(libmedia_transfer_protocol::librtc::Dtls * dtls)
	  {
#if TEST_RTC_PLAY
		  if (capture_type_)
		  {
			  if (video_encoder_thread_)
			  {
				  video_encoder_thread_->Stop();
			  }
			  if (x264_encoder_)
			  {
				  //	x264_encoder_->SetSendFrame(nullptr);
				  x264_encoder_->Stop();
			  }
			  if (capturer_track_source_)
			  {
				  //	capturer_track_source_->set_catprue_callback(nullptr, nullptr);
				  capturer_track_source_->Stop();
			  }
		  }
		 // GetSession()->CloseUser()
#endif //
		   
		  std::string session_name = GetSession()->SessionName();
		  GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE,[this, session_name]() {
			  std::shared_ptr<PlayRtcUser> slef = std::dynamic_pointer_cast<PlayRtcUser>(shared_from_this());
			  RtcService::GetInstance().RemovePlayUser(slef);
			  GbMediaService::GetInstance().CloseSession(session_name);
		  });
		 // 
	  }
#if TEST_RTC_PLAY
	  void PlayRtcUser::SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> encoded_image)
	  {
		  GBMEDIASERVER_LOG(LS_INFO);
		//  return;
		  // 视频的频率90000, 1s中90000份 1ms => 90
		  uint32_t rtp_timestamp = encoded_image->Timestamp() * 90;
		   

		  libmedia_transfer_protocol::RtpPacketizer::PayloadSizeLimits   limits;
		  libmedia_transfer_protocol::RTPVideoHeader   rtp_video_hreader;
		   
		  webrtc::RTPVideoHeaderH264  h;
		  // 多包和分包
		  h.packetization_mode = webrtc::H264PacketizationMode::NonInterleaved;
		  rtp_video_hreader.video_type_header = h;
		  absl::optional<libmedia_codec::VideoCodecType> video_type = libmedia_codec::kVideoCodecH264;
	 
		 std::unique_ptr<libmedia_transfer_protocol::RtpPacketizer> packetizer = libmedia_transfer_protocol::RtpPacketizer::Create(
		   video_type, rtc::ArrayView<const uint8_t>(encoded_image->data(), encoded_image->size()),
		   limits, rtp_video_hreader);
		 
		  // std::vector< std::unique_ptr<libmedia_transfer_protocol::RtpPacketToSend>>  packets;
	 
		  int32_t  number_packet = packetizer->NumPackets();
		  for (int32_t i = 0; i < number_packet; ++i)
		  {
			   
			auto  single_packet =
			std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(&rtp_header_extension_map_);
				 
			  single_packet->SetPayloadType(sdp_.GetVideoPayloadType());
			  single_packet->SetTimestamp(rtp_timestamp);
			  single_packet->SetSsrc(sdp_.VideoSsrc());
			  single_packet->ReserveExtension<libmedia_transfer_protocol::TransportSequenceNumber>();
			
			 if (!packetizer->NextPacket(single_packet.get())) 
			 {
				break;
			 }
			//  //int16_t   packet_id = transprot_seq_++;
			  single_packet->SetSequenceNumber(video_seq_++);
			  single_packet->set_packet_type(libmedia_transfer_protocol::RtpPacketMediaType::kVideo);
			 // single_packet->SetExtension<libmedia_transfer_protocol::TransportSequenceNumber>(video_seq_);
			  //AddPacketToTransportFeedback(packet_id, single_packet.get());
			  //if (video_send_stream_) {
			  //	video_send_stream_->UpdateRtpStats(single_packet, false, false);
			  //}

			  //AddVideoCache(single_packet);
			  // 发送数据包
			  // TODO, transport_name此处写死，后面可以换成变量
		  //	SendPacket("audio",  single_packet.get() );
			  //std::unique_ptr<libmedia_transfer_protocol::RtpPacketToSend> packet =
			  //	std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(*single_packet);
			//  srtp_session_.RtpProtect()
			  rtc::Buffer f(single_packet->data(), single_packet->size());
			  f.SetSize(single_packet->size());
			  rtc::Buffer   bf = srtp_session_.RtpProtect(f);
			  rtc::CopyOnWriteBuffer w(bf);

			  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(w, remote_address_, rtc::PacketOptions());
		     //packets.push_back( std::move(single_packet));
		  }
		//  return;
		 // GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(std::move(packets), remote_address_, rtc::PacketOptions());
		 // transport_send_->EnqueuePacket(std::move(packets));
	  }
#endif //
	  UserType   PlayRtcUser::GetUserType() const
	  {
		  return UserType::kUserTypePlayerWebRTC;
	  }
}