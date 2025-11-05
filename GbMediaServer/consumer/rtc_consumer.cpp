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
 
#include <random>
#include "consumer/rtc_consumer.h"
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
#include "common_video/h264/h264_common.h"
#include "server/stream.h"
namespace gb_media_server
{
	RtcConsumer::RtcConsumer(     std::shared_ptr<Stream> &stream,   std::shared_ptr<Session> &s)
	: RtcInterface ()
	, Consumer(stream, s)
		
		
	{
	
		
		uint32_t audio_ssrc = RtcInterface::GetSsrc(10);
		uint32_t video_ssrc = audio_ssrc + 1;

		
		sdp_.SetAudioSsrc(audio_ssrc);
		sdp_.SetVideoSsrc(video_ssrc);
		//dtls_certs_.Init();
		//dtls_.Init();
		//dtls_.SignalDtlsSendPakcet.connect(this, &PlayRtcUser::OnDtlsSendPakcet);
		//dtls_.SignalDtlsHandshakeDone.connect(this, &PlayRtcUser::OnDtlsHandshakeDone);
		//dtls_.SignalDtlsClose.connect(this, &PlayRtcUser::OnDtlsClosed);

		 
		dtls_.SignalDtlsConnecting.connect(this, &RtcConsumer::OnDtlsConnecting);
		dtls_.SignalDtlsConnected.connect(this, &RtcConsumer::OnDtlsConnected);
		dtls_.SignalDtlsClose.connect(this, &RtcConsumer::OnDtlsClosed);
		dtls_.SignalDtlsFailed.connect(this, &RtcConsumer::OnDtlsFailed);
		dtls_.SignalDtlsSendPakcet.connect(this, &RtcConsumer::OnDtlsSendPakcet);
		dtls_.SignalDtlsApplicationDataReceived.connect(this, &RtcConsumer::OnDtlsApplicationDataReceived);
		






		//sdp_.SetFingerprint(dtls_.Fingerprint());
		sdp_.SetLocalFingerprint(libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Fingerprints());
		// 本地ip port 
		sdp_.SetServerAddr(GbMediaService::GetInstance().RtpWanIp());
		sdp_.SetServerPort(GbMediaService::GetInstance().RtpPort());
		sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);


	
		//rtp_header_extension_map_.Register<libmedia_transfer_protocol::TransportSequenceNumber>(libmedia_transfer_protocol::kRtpExtensionTransportSequenceNumber);
	}
	RtcConsumer:: ~RtcConsumer(){
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		
		//dtls_.SignalDtlsSendPakcet.disconnect(this);
		//dtls_.SignalDtlsHandshakeDone.disconnect(this);
		//dtls_.SignalDtlsClose.disconnect(this);

		if (srtp_send_session_)
		{
			delete srtp_send_session_;
			srtp_send_session_ = nullptr;
		}
		if (srtp_recv_session_)
		{
			delete  srtp_recv_session_;
			srtp_recv_session_ = nullptr;
		}

		dtls_done_ = false;
		if (muxer_)
		{
			delete muxer_;
			muxer_ = nullptr;
		}
		if (srtp_send_session_)
		{
			srtp_send_session_->RemoveStream(sdp_.VideoSsrc());
			srtp_send_session_->RemoveStream(sdp_.AudioSsrc());
		}
		dtls_.disconnect_all();
		
		

		
		
	}

 
	

	bool RtcConsumer::ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string &sdp) {
		sdp_.SetSdpType(rtc_sdp_type);
		return sdp_.Decode(sdp);
	}
	const std::string &RtcConsumer::LocalUFrag() const {
		return sdp_.GetLocalUFrag();
	}
	const std::string &RtcConsumer::LocalPasswd() const {
		return sdp_.GetLocalPasswd();
	}
	const std::string &RtcConsumer::RemoteUFrag() const {
		return sdp_.GetRemoteUFrag();
	}
	std::string RtcConsumer::BuildAnswerSdp() {
		//sdp_.SetFingerprint(dtls_.Fingerprint());
		return sdp_.Encode();
	}

	void RtcConsumer::MayRunDtls()
	{
		dtls_.SetRemoteFingerprint(sdp_.GetRemoteFingerprint());

		// client and server 
		std::string  role = sdp_.GetRemoteRole();
		 //// role = "active" / "passive" / "actpass" / "holdconn"
		/*a = setup 主要是表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器
		a = setup:actpass 既可以是客户端，也可以是服务器
		a = setup : active 客户端
		a = setup : passive 服务器
		由客户端先发起client hello*/
		libmedia_transfer_protocol::libssl::Role   local_role = libmedia_transfer_protocol::libssl::Role::SERVER;
		if (role == "actpass" || role == "active")
		{
			local_role = libmedia_transfer_protocol::libssl::Role::SERVER;
		}
		else if (role == "passive")
		{
			//远端是服务时候就需要自动发送Hello 
			local_role = libmedia_transfer_protocol::libssl::Role::CLIENT;
		}
		GBMEDIASERVER_LOG(LS_INFO) << "remote role:"<< role <<" , local_role :" << (int32_t)local_role;
		dtls_.Run(local_role);
	}

#if 0
	  std::string RtcConsumer::GetUFrag(int size) {
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
	  uint32_t RtcConsumer::GetSsrc(int size)
	  {
		  static std::mt19937 mt{ std::random_device{}() };
		  static std::uniform_int_distribution<> rand(10000000, 99999999);

		  return rand(mt);
	  }
#endif // 

	   
	  void RtcConsumer::OnDtlsRecv(const uint8_t *buf, size_t size)
	  {
		  dtls_.OnRecv(buf, size);
	  }
	  void RtcConsumer::OnSrtpRtp(  uint8_t * data, size_t size)
	  {
	  }
	  void RtcConsumer::OnSrtpRtcp(  uint8_t * data, size_t size)
	  {
	  }
  

	  void RtcConsumer::OnVideoFrame(const libmedia_codec::EncodedImage &frame)
	  {
		  if (!dtls_done_)
		  {
			  return;
		  }
		  //if (dtls_.)

		  if (frame.size() < 5)
		  {
			  return;
		  }
		  
		  uint32_t rtp_timestamp = frame.Timestamp() * 90;
		  
		  if (!srtp_send_session_)
		  {
			  GBMEDIASERVER_LOG(LS_WARNING) << "ignoring RTP packet due to non sending SRTP session";
			  return;
		  }

		  libmedia_transfer_protocol::RtpPacketizer::PayloadSizeLimits   limits;
		  libmedia_transfer_protocol::RTPVideoHeader   rtp_video_hreader;
		 
		  webrtc::RTPVideoHeaderH264  h;
		  // 多包和分包
		  h.packetization_mode = webrtc::H264PacketizationMode::NonInterleaved;
		  rtp_video_hreader.video_type_header = h;
		  absl::optional<libmedia_codec::VideoCodecType> video_type = libmedia_codec::kVideoCodecH264;
		  // for (int32_t nal = 0; nal < fragments_count; ++nal)
		  {
			  std::unique_ptr<libmedia_transfer_protocol::RtpPacketizer> packetizer = 
				  libmedia_transfer_protocol::RtpPacketizer::Create(
				  video_type, rtc::ArrayView<const uint8_t>(frame.data() /*+nalus[nal].payload_start_offset */, frame.size()
					   /*nalus[nal].payload_size*/ ),
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



				  const uint8_t *data = single_packet->data();
				  size_t   len = single_packet->size();
				  if (!srtp_send_session_->EncryptRtp(&data, &len))
				  {
					  continue;
				  }
				  

				  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), rtc_remote_address_, rtc::PacketOptions());
				  //packets.push_back( std::move(single_packet));
			  }
		  }
	  }
	  void RtcConsumer::OnAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	  {
#if 0
		  // TODO@chensong  2025-10-24  AAC 转OPUS暂时不支持 后期支持
		  muxer_->EncodeAudio(frame);
#endif //

		  auto  single_packet =
			  std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(&rtp_header_extension_map_);
		  //GBMEDIASERVER_LOG(LS_INFO) << "audio size:" << frame.size() << ", pts: " << pts;
		  single_packet->SetPayloadType(sdp_.GetAudioPayloadType());
		  single_packet->SetTimestamp(pts);
		  single_packet->SetSsrc(sdp_.AudioSsrc());
		  single_packet->ReserveExtension<libmedia_transfer_protocol::TransportSequenceNumber>();
		  single_packet->SetMarker(true);
	 
		  single_packet->SetSequenceNumber(audio_seq_++);
		  single_packet->set_packet_type(libmedia_transfer_protocol::RtpPacketMediaType::kAudio);

		  uint8_t* audio_ptr =  single_packet->AllocatePayload(frame.size());
		  if (audio_ptr)
		  {
			  memcpy(audio_ptr, frame.data(), frame.size());
			  single_packet->SetPayloadSize(frame.size());
		  }
		  const uint8_t* data = single_packet->data();
		  size_t   len = single_packet->size();
		  if (!srtp_send_session_->EncryptRtp(&data, &len))
		  {
			 // continue;
			  return;
		  }


		  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), rtc_remote_address_, rtc::PacketOptions());
		  //packets.push_back( std::move(single_packet));
	  }

}