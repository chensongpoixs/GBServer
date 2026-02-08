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
 
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "gb_media_server_log.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_util.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_received.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"

#include "server/stream.h"
#include "producer/rtc_producer.h"
#include "server/session.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/video_rtp_depacketizer_h264.h"
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
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/stream.h"

namespace gb_media_server {


 
	RtcProducer::RtcProducer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		: RtcInterface()
		, Producer(  stream, s), 
		task_safety_(webrtc::PendingTaskSafetyFlag::CreateDetachedInactive()),

		recv_buffer_(new uint8_t[1024 * 1024 * 8])
	, recv_buffer_size_(0) 
		 ,nal_parse_(nullptr)
		, rtcp_context_recv_(new libmedia_transfer_protocol::librtcp::RtcpContextRecv())
		///,rtp_header_()
		//, extension_manager_()
	{
		//local_ufrag_ = GetUFrag(8);
		//local_passwd_ = GetUFrag(32);
		///uint32_t audio_ssrc = GetSsrc(10);
		//uint32_t video_ssrc = audio_ssrc + 1;

		//sdp_.SetLocalUFrag(local_ufrag_);
		//sdp_.SetLocalPasswd(local_passwd_);
		//sdp_.SetAudioSsrc(audio_ssrc);
		//sdp_.SetVideoSsrc(video_ssrc);
		//dtls_certs_.Init();
		//dtls_.Init();
		//dtls_.SignalDtlsSendPakcet.connect(this, &PlayRtcUser::OnDtlsSendPakcet);
		//dtls_.SignalDtlsHandshakeDone.connect(this, &PlayRtcUser::OnDtlsHandshakeDone);
		//dtls_.SignalDtlsClose.connect(this, &PlayRtcUser::OnDtlsClosed);


		dtls_.SignalDtlsConnecting.connect(this, &RtcProducer::OnDtlsConnecting);
		dtls_.SignalDtlsConnected.connect(this, &RtcProducer::OnDtlsConnected);
		dtls_.SignalDtlsClose.connect(this, &RtcProducer::OnDtlsClosed);
		dtls_.SignalDtlsFailed.connect(this, &RtcProducer::OnDtlsFailed);
		dtls_.SignalDtlsSendPakcet.connect(this, &RtcProducer::OnDtlsSendPakcet);
		dtls_.SignalDtlsApplicationDataReceived.connect(this, &RtcProducer::OnDtlsApplicationDataReceived);







		//sdp_.SetFingerprint(dtls_.Fingerprint());
		sdp_.SetLocalFingerprint(libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Fingerprints());
		// 本地ip port 
		//sdp_.SetServerAddr(GbMediaService::GetInstance().RtpWanIp());
		//sdp_.SetServerPort(GbMediaService::GetInstance().RtpPort());
		sdp_.SetServerAddr(YamlConfig::GetInstance().GetRtcServerConfig().ips.at(0));
		sdp_.SetServerPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);
		nal_parse_ = libmedia_codec::NalParseFactory::Create( 
			libmedia_codec::ENalParseType::ENalH264Prase );;
		
		rtp_header_.extension.hasAbsoluteSendTime = true;
		rtp_header_.extension.hasTransportSequenceNumber = true;


		
	
	}
	RtcProducer::~RtcProducer()
	{
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
		 
		if (srtp_send_session_)
		{
			srtp_send_session_->RemoveStream(sdp_.VideoSsrc());
			srtp_send_session_->RemoveStream(sdp_.AudioSsrc());
		}
		dtls_.disconnect_all();

		if (recv_buffer_)
		{
			recv_buffer_.reset();
			recv_buffer_ = nullptr;
		}
	}
	void RtcProducer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
	{
#if 0
		memcpy(recv_buffer_ + recv_buffer_size_, buffer1.data(), buffer1.size());
		recv_buffer_size_ += buffer1.size();
		//recv_buffer_.SetData(buffer1);
		int32_t   parse_size = 0;

		 
		while (recv_buffer_size_ - parse_size > 2)
		{ 
			int16_t  payload_size = libmedia_transfer_protocol::ByteReader<int16_t>::ReadBigEndian((&recv_buffer_[parse_size]));
			 
			if ((recv_buffer_size_ - parse_size) < (payload_size + 2))
			{
				// 当不不够一个完整包需要继续等待下一个包的到来
				//GBMEDIASERVER_LOG(LS_INFO) << "tcp tail small !!!  (read_bytes -parse_size:" << (recv_buffer_size_ - parse_size) << ") payload_size:" << payload_size;
				break;
			}
			parse_size += 2;  
			if (libmedia_transfer_protocol::IsRtpPacket(rtc::ArrayView<uint8_t>(recv_buffer_ + parse_size, payload_size)))
			{
				libmedia_transfer_protocol::RtpPacketReceived  rtp_packet_received; 
				bool ret = rtp_packet_received.Parse(recv_buffer_  + parse_size, payload_size);
				if (!ret)
				{ 
					GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed !!! size:" << (recv_buffer_size_ - parse_size); //<< "  , hex :" << rtc::hex_encode((const char *)(buffer.begin() + paser_size), (size_t)(read_bytes - paser_size));
				}
				else
				{
					//RTC_LOG(LS_INFO) << "rtp info :" << rtp_packet_received.ToString();
					if (rtp_packet_received.PayloadType() == 96)
					{
						//mpeg_decoder_->parse( rtp_packet_received.payload().data(), rtp_packet_received.payload_size());; 
					} 
				}
			}
			else if (libmedia_transfer_protocol::IsRtcpPacket(rtc::ArrayView<uint8_t>(recv_buffer_ + parse_size, payload_size/*read_bytes - paser_size*/)))
			{
				libmedia_transfer_protocol::rtcp::CommonHeader rtcp_block;  //rtcp_packet;
				bool ret = rtcp_block.Parse(recv_buffer_ + parse_size, payload_size/* read_bytes - paser_size*/);
				if (!ret)
				{
					GBMEDIASERVER_LOG(LS_WARNING) << "rtcp parse failed !!!";
				}
				//else
				{
					//parse_size += payload_size;
					//	RTC_LOG(LS_INFO) << "rtcp info :" << rtcp_block.ToString();
				}
			}
			else
			{
				GBMEDIASERVER_LOG(LS_ERROR) << " not know type --> : payload_size: " << payload_size;
				//parse_size += payload_size;
			}
			parse_size += payload_size;
 
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "read_bytes:" << recv_buffer_size_ << ", parse_size:" << parse_size;
		if (recv_buffer_size_ - parse_size > 0)
		{
			//memcpy((char *)recv_buffer_.begin(), buffer.data() + parse_size, (buffer.size() - parse_size));
			recv_buffer_size_ -= parse_size;;
			memmove(recv_buffer_, recv_buffer_+ parse_size,recv_buffer_size_);
			 
		}
		else
		{
			recv_buffer_size_ = 0;
			parse_size = 0;
			//memcpy((char *)recv_buffer_.begin(), buffer.begin() + parse_size, (read_bytes - parse_size));
			//recv_buffer_size_ = read_bytes - parse_size;
		}
		
#endif // recv_buffer_size_
	}





	void RtcProducer::OnTimer()
	{
		/*
		 network_thread_->PostDelayedTask(
        ToQueuedTask(task_safety_,
                     [this, recheck = *result.recheck_event]() {
                       SortConnectionsAndUpdateState(recheck);
                     }),
        result.recheck_event->recheck_delay_ms);
		*/
#if 1
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayedTask(ToQueuedTask(task_safety_,
		// gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayed(RTC_FROM_HERE, 
			[this]() {
				if (!dtls_done_)
				{

					return ;
				}
				//if (rtc::SystemTimeMillis() - rtcp_rr_timestamp_ > 4000)
				{
					rtc::Buffer buffer = rtcp_context_recv_->createRtcpRR(sdp_.VideoSsrc(), sdp_.VideoSsrc());
					GBMEDIASERVER_LOG(LS_INFO) << "Send RR";
					SendSrtpRtcp(buffer.data(), buffer.size());
					rtcp_rr_timestamp_ = rtc::SystemTimeMillis();
				}
				{
				
					stream_status_ = stream_status_? false:true;
				//	SetStreamStatus(stream_status_);
				}
 
			OnTimer();
		}), 5000);

#endif //
		// PostDelayed(RTC_FROM_HERE, milliseconds, &queued_task_handler_,
		/*id=*///0,
		//	new ScopedMessageData<webrtc::QueuedTask>(std::move(task)));
	//	gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayed(RTC_FROM_HERE, 5000,   this, );

	}

	bool RtcProducer::ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp) {
		sdp_.SetSdpType(rtc_sdp_type);
		return sdp_.Decode(sdp);
	}
	const std::string& RtcProducer::LocalUFrag() const {
		return sdp_.GetLocalUFrag();
	}
	const std::string& RtcProducer::LocalPasswd() const {
		return sdp_.GetLocalPasswd();
	}
	const std::string& RtcProducer::RemoteUFrag() const {
		return sdp_.GetRemoteUFrag();
	}
	std::string RtcProducer::BuildAnswerSdp() {
		//sdp_.SetFingerprint(dtls_.Fingerprint());
		return sdp_.Encode();
	}

	void RtcProducer::MayRunDtls()
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
		GBMEDIASERVER_LOG(LS_INFO) << "remote role:" << role << " , local_role :" << (int32_t)local_role;
		dtls_.Run(local_role);
	}

	void RtcProducer::OnDtlsRecv(const uint8_t* buf, size_t size)
	{
		dtls_.OnRecv(buf, size);
	}
	void RtcProducer::OnSrtpRtp(  uint8_t* data, size_t size)
	{
		//GBMEDIASERVER_LOG_T_F(LS_INFO);
		if (!srtp_recv_session_->DecryptSrtp((uint8_t*)data, (size_t*)&size))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "decrypt srtp failed  size : " <<size<<" !!!";
			return;
		}


		libmedia_transfer_protocol::RtpPacketReceived  rtp_packet_received(&extension_manager_);
		bool ret = rtp_packet_received.Parse(data , size);
		if (!ret)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed !!! size:" << size; //<< "  , hex :" << rtc::hex_encode((const char *)(buffer.begin() + paser_size), (size_t)(read_bytes - paser_size));
		}
		else
		{
			
			rtp_packet_received.GetHeader(&rtp_header_);
			//GBMEDIASERVER_LOG(LS_INFO) << rtp_header_.ToString() ;
		

			if (rtp_header_.extension.hasTransportSequenceNumber)
			{
				twcc_context_.onRtp(rtp_header_.ssrc, 
					rtp_header_.extension.transportSequenceNumber, 
					rtc::SystemTimeMillis());
			}
			if (rtp_packet_received.PayloadType() != sdp_.GetVideoPayloadType()) 
			{
				
				if (rtp_packet_received.PayloadType() == sdp_.GetAudioPayloadType())
				{

					GetStream()->AddAudioFrame(rtc::CopyOnWriteBuffer(rtp_packet_received.payload().data(), rtp_packet_received.payload_size())
						, rtp_packet_received.Timestamp()/90);
				}
				else
				{
					 GBMEDIASERVER_LOG(LS_INFO) << "payload_type:" << rtp_packet_received.PayloadType()
				 	<< ", ssrc:" << rtp_packet_received.Ssrc() << ", video payload type:" << sdp_.GetVideoPayloadType() 
						 ;

				}
				return;
			}
			/*RTC_LOG(LS_INFO) << "rtp info :" << rtp_packet_received.PayloadType() 
				<< ", seq:" << rtp_packet_received.SequenceNumber()
				<< ", masker:" << rtp_packet_received.Marker();*/
			if (rtcp_context_recv_ && rtp_packet_received.Ssrc() == sdp_.VideoSsrc())
			{
				// ntp_stamp : getStamp() * uint64_t(1000) / sample_rate
				rtcp_context_recv_->onRtp(rtp_packet_received.SequenceNumber(), rtp_packet_received.Timestamp(),
					rtp_packet_received.Timestamp()/ 90000, 90000,rtp_packet_received.payload_size());
			}
			//GBMEDIASERVER_LOG(LS_INFO) << " ssrc:" << rtp_packet_received.Ssrc() << ", payload_type:" << rtp_packet_received.PayloadType() << ", seq:" << rtp_packet_received.SequenceNumber()
			//	<< ", marker:" << rtp_packet_received.Marker() << ", payload_size:" << rtp_packet_received.payload_size();
			//memcpy(recv_buffer_ + recv_buffer_size_, rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
			//recv_buffer_size_ += rtp_packet_received.payload_size();
			nal_parse_->parse_packet(rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
			if (rtp_packet_received.Marker())
			{
				 
				libmedia_codec::EncodedImage encode_image;
				encode_image.SetTimestamp(rtp_packet_received.Timestamp()/90);
				encode_image.SetEncodedData(
					libmedia_codec::EncodedImageBuffer::Create(
						nal_parse_->buffer_stream_,
						nal_parse_->buffer_index_
						//video_parse->video_payload.data(),
						//video_parse->video_payload.size()
					));

#if 0

				static FILE* out_file_ptr = fopen("rtc_push.h264", "wb+");
				if (out_file_ptr)
				{
					char start_code[4] = {0x00, 0x00, 0x00, 0x01};
					//fwrite(start_code, 1, sizeof(start_code), out_file_ptr);
					fwrite(nal_parse_->buffer_stream_, 1, nal_parse_->buffer_index_, out_file_ptr);
					//fwrite(video_parse->video_payload.data(), 1, video_parse->video_payload.size(), out_file_ptr);
					fflush(out_file_ptr);
				}

#endif 
				//decoder_->Decode(encode_image, true, 1);
				GetStream()->AddVideoFrame(std::move(encode_image));
				nal_parse_->buffer_index_ = 0;
				//decoder_->Decode();
			}
		}

	}
	void RtcProducer::OnSrtpRtcp(  uint8_t* data, size_t size)
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
					return ;
				}
				 ++num_skipped_packets_;
				break;
			}

			//if (packet_type_counter_.first_packet_time_ms == -1)
			//	packet_type_counter_.first_packet_time_ms = clock_->TimeInMilliseconds();
			//RTC_LOG_F(LS_INFO) << "recvice RTCP TYPE = " << rtcp_block.type();
			switch (rtcp_block.type()) {
			case libmedia_transfer_protocol::rtcp::SenderReport::kPacketType:
			{
				//RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type();
				//HandleSenderReport(rtcp_block, packet_information);
				// 
				//received_blocks[packet_information->remote_ssrc].sender_report = true;
				libmedia_transfer_protocol::rtcp::SenderReport sender_report;
				if (!sender_report.Parse(rtcp_block)) {
					++num_skipped_packets_;
					return;
				}

				RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type() 
					<< ", ssrc:" << sender_report.sender_ssrc();
				if (rtcp_context_recv_ && sender_report.sender_ssrc() == sdp_.VideoSsrc())
				{
					rtcp_context_recv_->onRtcp(&sender_report);
				}
				break;
			}
			case libmedia_transfer_protocol::rtcp::ReceiverReport::kPacketType:
				   RTC_LOG_F(LS_INFO) << "recvice RR RTCP TYPE = " << rtcp_block.type();
				//HandleReceiverReport(rtcp_block, packet_information);
				break;
			case libmedia_transfer_protocol::rtcp::Sdes::kPacketType:
			{
				//RTC_LOG(LS_INFO) << "recvice SDES RTCP TYPE = " << rtcp_block.type();
				//HandleSdes(rtcp_block, packet_information);
				libmedia_transfer_protocol::rtcp::Sdes sdes;
				if (!sdes.Parse(rtcp_block)) {
					++num_skipped_packets_;

				}
				else
				{
					for (const libmedia_transfer_protocol::rtcp::Sdes::Chunk& chunk : sdes.chunks()) {
						RTC_LOG(LS_INFO) << "recvice SDES RTCP  ssrc: " << chunk.ssrc << ", cname:" << chunk.cname;
					}
				}
				break; 
			}
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
				RTC_LOG_F(LS_INFO) << "recvice rtpfb ";
				//switch (rtcp_block.fmt()) {
				//case rtcp::Nack::kFeedbackMessageType:
				//	//  RTC_LOG_F(LS_INFO) << "recvice rtpfb  nack RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleNack(rtcp_block, packet_information);
				//	break;
				//case rtcp::Tmmbr::kFeedbackMessageType:
				//	RTC_LOG(LS_INFO) << "recvice rtpfb  tmmbr RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleTmmbr(rtcp_block, packet_information);
				//	break;
				//case rtcp::Tmmbn::kFeedbackMessageType:
				//	RTC_LOG(LS_INFO) << "recvice rtpfb tmmbn RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleTmmbn(rtcp_block, packet_information);
				//	break;
				//case rtcp::RapidResyncRequest::kFeedbackMessageType:
				//	RTC_LOG(LS_INFO) << "recvice rtpfb rapidresy ync  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleSrReq(rtcp_block, packet_information);
				//	break;
				//case rtcp::TransportFeedback::kFeedbackMessageType:
				//	//  RTC_LOG_F(LS_INFO) << "recvice rtpfb transport feedback  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleTransportFeedback(rtcp_block, packet_information);
				//	break;
				//default:
				//	++num_skipped_packets_;
				//	break;
				//}
				break;
			case libmedia_transfer_protocol::rtcp::Psfb::kPacketType:
				RTC_LOG(LS_INFO) << "recvice psfb  pli";
				/*switch (rtcp_block.fmt()) {
				case rtcp::Pli::kFeedbackMessageType:
					RTC_LOG(LS_INFO) << "recvice psfb  pli  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
					HandlePli(rtcp_block, packet_information);
					break;
				case rtcp::Fir::kFeedbackMessageType:
					RTC_LOG(LS_INFO) << "recvice psfb  fir  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
					HandleFir(rtcp_block, packet_information);
					break;
				case rtcp::Psfb::kAfbMessageType:
					RTC_LOG(LS_INFO) << "recvice psfb  psfb  af  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
					HandlePsfbApp(rtcp_block, packet_information);
					break;
				default:
					++num_skipped_packets_;
					break;
				}*/
				break;
			default:
				 ++num_skipped_packets_;
				break;
			}
		}
		//OnRecv(data, size);
	}

#if 0
	std::string RtcProducer::GetUFrag(int size) {
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
	uint32_t RtcProducer::GetSsrc(int size)
	{
		static std::mt19937 mt{ std::random_device{}() };
		static std::uniform_int_distribution<> rand(10000000, 99999999);

		return rand(mt);
	}
#endif // 

	void RtcProducer::RequestKeyFrame()
	{
		if (request_key_frame_ > std::time(nullptr))
		{
			return;
		}
		request_key_frame_ = std::time(nullptr) + 3;
		///////////////////////////////////////////////////////////////////////////
	////                         IDR Request

	//     关键帧也叫做即时刷新帧，简称IDR帧。对视频来说，IDR帧的解码无需参考之前的帧，因此在丢包严重时可以通过发送关键帧请求进行画面的恢复。
	// 关键帧的请求方式分为三种：RTCP FIR反馈（Full intra frame request）、RTCP PLI 反馈（Picture Loss Indictor）或SIP Info消息，
	//							具体使用哪种可通过协商确定.

	///////////////////////////////////////////////////////////////////////////
		//if (this->params.usePli)
		{

			std::unique_ptr< libmedia_transfer_protocol::rtcp::Pli> pli = std::make_unique< libmedia_transfer_protocol::rtcp::Pli>();
			pli->SetSenderSsrc(sdp_.VideoSsrc());
			pli->SetMediaSsrc(sdp_.VideoSsrc());
			GBMEDIASERVER_LOG(LS_INFO) << "send rquest pli ";
			libmedia_transfer_protocol::rtcp::CompoundPacket compound;               // Builds a compound RTCP packet with
		    compound.Append(std::move(pli));                  // a receiver report, report block
		   // compound.Append(&fir);                 // and fir message.
		    rtc::Buffer packet = compound.Build();
			SendSrtpRtcp(packet.data(), packet.size());
			//SendImmediateFeedback(pli);
			//libmedia_transfer_protocol::rtcp::TransportFeedback  
			//MS_DEBUG_2TAGS(rtcp, rtx, "sending PLI [ssrc:%" PRIu32 "]", GetSsrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			//RTC::RTCP::FeedbackPsPliPacket packet(GetSsrc(), GetSsrc());

			//packet.Serialize(RTC::RTCP::Buffer);

			//this->pliCount++;

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
		//else if (this->params.useFir)
		{
			//MS_DEBUG_2TAGS(rtcp, rtx, "sending FIR [ssrc:%" PRIu32 "]", GetSsrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			//RTC::RTCP::FeedbackPsFirPacket packet(GetSsrc(), GetSsrc());
			//auto* item = new RTC::RTCP::FeedbackPsFirItem(GetSsrc(), ++this->firSeqNumber);
			//
			//packet.AddItem(item);
			//packet.Serialize(RTC::RTCP::Buffer);
			//
			//this->firCount++;

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
	}

	void RtcProducer::SetStreamStatus(bool status)
	{
		std::unique_ptr< libmedia_transfer_protocol::rtcp::Stream> stream = std::make_unique< libmedia_transfer_protocol::rtcp::Stream>();
		stream->SetSenderSsrc(sdp_.VideoSsrc());
	//	stream->setm(sdp_.VideoSsrc());
		stream->SetStatus(status);
		GBMEDIASERVER_LOG(LS_INFO) << " set  ssrc:"<<sdp_.VideoSsrc() <<", send stream status  :" << status;
		libmedia_transfer_protocol::rtcp::CompoundPacket compound;               // Builds a compound RTCP packet with
		compound.Append(std::move(stream));                  // a receiver report, report block
	   // compound.Append(&fir);                 // and fir message.
		rtc::Buffer packet = compound.Build();
		SendSrtpRtcp(packet.data(), packet.size());
	}
	void RtcProducer::OnDataChannel(
		const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params,
		uint32_t ppid, const uint8_t* msg, size_t len)
	{
		if (sctp_)
		{
			sctp_->SendSctpMessage(params, ppid, msg, len);
		}
	}

	void RtcProducer::OnSctpAssociationMessageReceived(
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