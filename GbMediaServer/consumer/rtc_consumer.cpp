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

#include "gb_media_server_log.h"
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
#include "utils/yaml_config.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/receiver_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/psfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sender_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sdes.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/extended_reports.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/bye.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/rtpfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/pli.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/fir.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/nack.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/remb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/transport_feedback.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/tmmbn.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/tmmbr.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/tmmb_item.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/rapid_resync_request.h"

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
		sdp_.SetVideoRtxSsrc(video_ssrc + 1);
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
		sdp_.SetServerAddr(YamlConfig::GetInstance().GetRtcServerConfig().ips.at(0));
		sdp_.SetServerPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
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

	void RtcConsumer::RequestKeyFrame()
	{
		GetSession()->ConsumerRequestKeyFrame();
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
		  //GBMEDIASERVER_LOG_T_F(LS_INFO);
		  if (!srtp_recv_session_->DecryptSrtcp((uint8_t*)data, (size_t*)&size))
		  {
			  GBMEDIASERVER_LOG_T_F(LS_WARNING) << "decrypt srtcp failed !!!";
			  return;
		  }
 
		  rtc::ArrayView<const uint8_t>  packet(data, size);
		  libmedia_transfer_protocol::rtcp::CommonHeader rtcp_block;
		  // If a sender report is received but no DLRR, we need to reset the
	// roundTripTime stat according to the standard, see
	// https://www.w3.org/TR/webrtc-stats/#dom-rtcremoteoutboundrtpstreamstats-roundtriptime
		  struct RtcpReceivedBlock {
			  bool sender_report = false;
			  bool dlrr = false;
		  };
		  // For each remote SSRC we store if we've received a sender report or a DLRR
		  // block.
		  webrtc::flat_map<uint32_t, RtcpReceivedBlock> received_blocks;
		  for (const uint8_t* next_block = packet.begin(); next_block != packet.end();
			  next_block = rtcp_block.NextPacket()) {
			  ptrdiff_t remaining_blocks_size = packet.end() - next_block;
			  RTC_DCHECK_GT(remaining_blocks_size, 0);
			  if (!rtcp_block.Parse(next_block, remaining_blocks_size)) {
				  if (next_block == packet.begin()) {
					  // Failed to parse 1st header, nothing was extracted from this packet.
					  RTC_LOG(LS_WARNING) << "Incoming invalid RTCP packet";
					  return;
				  }
				//  ++num_skipped_packets_;
				  break;
			  }

			  //if (packet_type_counter_.first_packet_time_ms == -1)
			  //	packet_type_counter_.first_packet_time_ms = clock_->TimeInMilliseconds();
			  //RTC_LOG_F(LS_INFO) << "recvice RTCP TYPE = " << rtcp_block.type();
			  switch (rtcp_block.type()) {
			  case libmedia_transfer_protocol::rtcp::SenderReport::kPacketType:
			  {
				 // RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type();
				  //HandleSenderReport(rtcp_block, packet_information);
				  // 
				  //received_blocks[packet_information->remote_ssrc].sender_report = true;
				  libmedia_transfer_protocol::rtcp::SenderReport sender_report;
				  if (!sender_report.Parse(rtcp_block)) {
					 // ++num_skipped_packets_;
					  return;
				  }

				  RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type()
					  << ", ssrc:" << sender_report.sender_ssrc();
				/*  if (rtcp_context_recv_ && sender_report.sender_ssrc() == sdp_.VideoSsrc())
				  {
					  rtcp_context_recv_->onRtcp(&sender_report);
				  }*/
				  break;
			  }
			  case libmedia_transfer_protocol::rtcp::ReceiverReport::kPacketType:
				  RTC_LOG_F(LS_INFO) << "recvice RR RTCP TYPE = " << rtcp_block.type();
				  //HandleReceiverReport(rtcp_block, packet_information);
				  break;
			  case libmedia_transfer_protocol::rtcp::Sdes::kPacketType:
				  RTC_LOG(LS_INFO) << "recvice SDES RTCP TYPE = " << rtcp_block.type();
				  //HandleSdes(rtcp_block, packet_information);
				  break;
			  case libmedia_transfer_protocol::rtcp::ExtendedReports::kPacketType: {
				  RTC_LOG(LS_INFO) << "recvice ExtenderR RTCP TYPE = " << rtcp_block.type();
				  //bool contains_dlrr = false;
				  //uint32_t ssrc = 0;
				  //HandleXr(rtcp_block, packet_information, contains_dlrr, ssrc);
				  //if (contains_dlrr) {
				  //	received_blocks[ssrc].dlrr = true;
				  //}
				  break;
			  }
			  case libmedia_transfer_protocol::rtcp::Bye::kPacketType:
				  RTC_LOG(LS_INFO) << "recvice Bye RTCP TYPE = " << rtcp_block.type();
				  //HandleBye(rtcp_block);
				  break;
			  case libmedia_transfer_protocol::rtcp::App::kPacketType:
				  RTC_LOG(LS_INFO) << "recvice App RTCP TYPE = " << rtcp_block.type();
				  //HandleApp(rtcp_block, packet_information);
				  break;
			  case libmedia_transfer_protocol::rtcp::Rtpfb::kPacketType:
				  //RTC_LOG_F(LS_INFO) << "recvice rtpfb ";
				  switch (rtcp_block.fmt()) {
					  case libmedia_transfer_protocol::rtcp::Nack::kFeedbackMessageType:
					  {
						  RTC_LOG_F(LS_INFO) << "recvice rtpfb  nack RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						  //HandleNack(rtcp_block, packet_information);
						  libmedia_transfer_protocol::rtcp::Nack nack;
						  if (!nack.Parse(rtcp_block)) 
						  {
							  GBMEDIASERVER_LOG_T_F(LS_WARNING) << "parse recvice rtpfb nack failed !!!";
							 // ++num_skipped_packets_;
							  continue;
						  }
						  break;
					  }
					  case libmedia_transfer_protocol::rtcp::Tmmbr::kFeedbackMessageType:
				  		RTC_LOG(LS_INFO) << "recvice rtpfb  tmmbr RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleTmmbr(rtcp_block, packet_information);
				  		break;
					  case libmedia_transfer_protocol::rtcp::Tmmbn::kFeedbackMessageType:
				  		RTC_LOG(LS_INFO) << "recvice rtpfb tmmbn RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleTmmbn(rtcp_block, packet_information);
				  		break;
					  case libmedia_transfer_protocol::rtcp::RapidResyncRequest::kFeedbackMessageType:
				  		RTC_LOG(LS_INFO) << "recvice rtpfb rapidresy ync  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleSrReq(rtcp_block, packet_information);
				  		break;
					  case libmedia_transfer_protocol::rtcp::TransportFeedback::kFeedbackMessageType:
				  		 // RTC_LOG_F(LS_INFO) << "recvice rtpfb transport feedback  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleTransportFeedback(rtcp_block, packet_information);
				  		break;
					  default:
				  		//++num_skipped_packets_;
				  		break;
				  }
				  break;
			  case libmedia_transfer_protocol::rtcp::Psfb::kPacketType:
				 // RTC_LOG(LS_INFO) << "recvice psfb  pli";
				   switch (rtcp_block.fmt()) {
					  case libmedia_transfer_protocol::rtcp::Pli::kFeedbackMessageType:
						  RTC_LOG(LS_INFO) << "recvice psfb  pli  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						 // HandlePli(rtcp_block, packet_information);
						  RequestKeyFrame();
						  break;
					  case libmedia_transfer_protocol::rtcp::Fir::kFeedbackMessageType:
						  RTC_LOG(LS_INFO) << "recvice psfb  fir  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						  //HandleFir(rtcp_block, packet_information);
						  RequestKeyFrame();
						  break;
					  case libmedia_transfer_protocol::rtcp::Psfb::kAfbMessageType:
						  RTC_LOG(LS_INFO) << "recvice psfb  psfb  af  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						  //HandlePsfbApp(rtcp_block, packet_information);
						  break;
					  default:
						 // ++num_skipped_packets_;
						  break;
				   } 
				  break;
			  default:
				 // ++num_skipped_packets_;
				  break;
			  }
		  }
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
					  std::make_shared<libmedia_transfer_protocol::RtpPacketToSend>(&extension_manager_);

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


#if  0
				  const uint8_t *data = single_packet->data();
				  size_t   len = single_packet->size();
				  if (!srtp_send_session_->EncryptRtp(&data, &len))
				  {
					  continue;
				  }
				  

				  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), rtc_remote_address_, rtc::PacketOptions());
				  //packets.push_back( std::move(single_packet));
#else
				  SendSrtpRtp((uint8_t*)single_packet->data(), single_packet->size());

#endif  
				  AddVideoPacket(single_packet);
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
			  std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(&extension_manager_);
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
#if 0
		  const uint8_t* data = single_packet->data();
		  size_t   len = single_packet->size();
		  if (!srtp_send_session_->EncryptRtp(&data, &len))
		  {
			 // continue;
			  return;
		  }


		  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), rtc_remote_address_, rtc::PacketOptions());
		  //packets.push_back( std::move(single_packet));
#else 

		  SendSrtpRtp((uint8_t*)single_packet->data(), single_packet->size());
#endif //
	  }

	  void RtcConsumer::OnDataChannel(
		  const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, 
		  uint32_t ppid, const uint8_t* msg, size_t len)
	  {
		  if (sctp_)
		  {
			  sctp_->SendSctpMessage(params, ppid, msg, len);
		  }
	  }
	  void RtcConsumer::OnSctpAssociationMessageReceived(
		  libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
		  uint16_t streamId,
		  uint32_t ppid,
		  const uint8_t* msg,
		  size_t len)
	  {
		  GBMEDIASERVER_LOG_T_F(LS_INFO) << " " << streamId << " " << ppid << " " << len << " "
			  << std::string((char*)msg, len);
		  libmedia_transfer_protocol::librtc::SctpStreamParameters params;
		  params.streamId = streamId;
		  GetSession()->AddDataChannel(params, ppid, msg, len);
	  }

}