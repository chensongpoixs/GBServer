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
#include "producer/gb28181_push_producer.h"
#include "server/session.h"
namespace gb_media_server {


 
	Gb28181PushProducer::Gb28181PushProducer( 
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		:Producer(  stream, s),
		mpeg_decoder_(nullptr),
		recv_buffer_(new uint8_t[1024 * 1024 * 8])
	, recv_buffer_size_(0){
		mpeg_decoder_ = std::make_unique<libmedia_transfer_protocol::libmpeg::MpegDecoder>();
		mpeg_decoder_->SignalRecvVideoFrame.connect(this, &Gb28181PushProducer::OnProcessVideoFrame);
		mpeg_decoder_->SignalRecvAudioFrame.connect(this, &Gb28181PushProducer::OnProcessAudioFrame);
		
	}
	Gb28181PushProducer::~Gb28181PushProducer()
	{
		if (mpeg_decoder_)
		{
			mpeg_decoder_->SignalRecvVideoFrame.disconnect_all();
			mpeg_decoder_.reset();
			//mpeg_decoder_->SignalRecvAudioFrame.disconnect(this);
		}
		if (recv_buffer_)
		{
			delete[]recv_buffer_;
			recv_buffer_ = nullptr;
		}
	}
	void Gb28181PushProducer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
	{

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
						mpeg_decoder_->parse( rtp_packet_received.payload().data(), rtp_packet_received.payload_size());; 
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
		
		
	}

	void Gb28181PushProducer::OnProcessVideoFrame(libmedia_codec::EncodedImage frame)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "";

		//rtc::CopyOnWriteBuffer  buffer;
		//buffer.AppendData(frame);
#if 0
		static FILE * out_file_ptr = fopen("ps.h264", "wb+");
		if (out_file_ptr)
		{
			fwrite(buffer.data(), buffer.size(), 1, out_file_ptr);
			fflush(out_file_ptr);
		}
#endif //
		GetStream()->AddVideoFrame(frame);

	}
	void Gb28181PushProducer::OnProcessAudioFrame(rtc::CopyOnWriteBuffer frame)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "";
#if 0
		static FILE * out_file_ptr = fopen("ps_audio.aac", "wb+");
		if (out_file_ptr)
		{
			fwrite(frame.data(), 1, frame.size(), out_file_ptr);
			fflush(out_file_ptr);
		}
#endif // 0
		GetStream()->AddAudioFrame(frame);
	}
	 
}