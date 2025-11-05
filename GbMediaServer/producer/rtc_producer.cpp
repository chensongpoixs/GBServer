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



 ******************************************************************************/
 
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_util.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_received.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"

#include "server/stream.h"
#include "producer/rtc_producer.h"
#include "server/session.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/video_rtp_depacketizer_h264.h"


namespace gb_media_server {


 
	RtcProducer::RtcProducer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		: RtcInterface()
		, Producer(  stream, s), 
		recv_buffer_(new uint8_t[1024 * 1024 * 8])
	, recv_buffer_size_(0) 
		 ,nal_parse_(nullptr)
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
		sdp_.SetServerAddr(GbMediaService::GetInstance().RtpWanIp());
		sdp_.SetServerPort(GbMediaService::GetInstance().RtpPort());
		sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);
		nal_parse_ = libmedia_codec::NalParseFactory::Create( 
			libmedia_codec::ENalParseType::ENalH264Prase );;
		
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
			delete[]recv_buffer_;
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
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "decrypt srtp failed !!!";
			return;
		}


		libmedia_transfer_protocol::RtpPacketReceived  rtp_packet_received;
		bool ret = rtp_packet_received.Parse(data , size);
		if (!ret)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed !!! size:" << size; //<< "  , hex :" << rtc::hex_encode((const char *)(buffer.begin() + paser_size), (size_t)(read_bytes - paser_size));
		}
		else
		{
			//RTC_LOG(LS_INFO) << "rtp info :" << rtp_packet_received.ToString();
			//if (rtp_packet_received.PayloadType() == 96)
			//{
			//	//mpeg_decoder_->parse( rtp_packet_received.payload().data(), rtp_packet_received.payload_size());; 
			//}
			if (rtp_packet_received.PayloadType() != sdp_.GetVideoPayloadType()) 
			{
				//GBMEDIASERVER_LOG(LS_INFO) << "payload_type:" << rtp_packet_received.PayloadType()
				//	<< ", ssrc:" << rtp_packet_received.Ssrc() << ", video payload type:" << sdp_.GetVideoPayloadType()  ;
				return;
			}
			GBMEDIASERVER_LOG(LS_INFO) << " ssrc:" << rtp_packet_received.Ssrc() << ", payload_type:" << rtp_packet_received.PayloadType() << ", seq:" << rtp_packet_received.SequenceNumber()
				<< ", marker:" << rtp_packet_received.Marker() << ", payload_size:" << rtp_packet_received.payload_size();
			//memcpy(recv_buffer_ + recv_buffer_size_, rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
			//recv_buffer_size_ += rtp_packet_received.payload_size();
			nal_parse_->parse_packet(rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
			if (rtp_packet_received.Marker())
			{
				 // libmedia_transfer_protocol::VideoRtpDepacketizerH264   video_rtp_depacket_h264;
				//absl::optional<libmedia_transfer_protocol::VideoRtpDepacketizer::ParsedRtpPayload>   video_parse =  video_rtp_depacket_h264.Parse(rtc::CopyOnWriteBuffer(recv_buffer_, recv_buffer_size_));
				
				//if (!video_parse)
				//{
				//	return;
				//}
				//recv_buffer_size_ = 0;
				
				libmedia_codec::EncodedImage encode_image;
				encode_image.SetTimestamp(rtp_packet_received.Timestamp());
				encode_image.SetEncodedData(
					libmedia_codec::EncodedImageBuffer::Create(
						nal_parse_->buffer_stream_,
						nal_parse_->buffer_index_
						//video_parse->video_payload.data(),
						//video_parse->video_payload.size()
					));

#if 1

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
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		if (!srtp_recv_session_->DecryptSrtcp((uint8_t*)data, (size_t*)&size))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "decrypt srtcp failed !!!";
			return;
		}
		libmedia_transfer_protocol::rtcp::CommonHeader rtcp_block;  //rtcp_packet;
		bool ret = rtcp_block.Parse(data, size);
		if (!ret)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "rtcp parse failed !!!";
		}
		else {
			GBMEDIASERVER_LOG(LS_INFO) << "rtcp info:" << rtcp_block.ToString();
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
	 
}