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
 
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_util.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_received.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"

#include "server/stream.h"
#include "producer/gb28181_producer.h"
#include "server/session.h"

#include "gb_media_server_log.h"

namespace gb_media_server {


 
	Gb28181Producer::Gb28181Producer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		:Producer(  stream, s),
		mpeg_decoder_(nullptr),
		recv_buffer_(new uint8_t[1024 * 1024 * 8])
	, recv_buffer_size_(0){
		mpeg_decoder_ = std::make_unique<libmedia_transfer_protocol::libmpeg::MpegDecoder>();
		mpeg_decoder_->SignalRecvVideoFrame.connect(this, &Gb28181Producer::OnProcessVideoFrame);
		mpeg_decoder_->SignalRecvAudioFrame.connect(this, &Gb28181Producer::OnProcessAudioFrame);
		
	}
	Gb28181Producer::~Gb28181Producer()
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
	void Gb28181Producer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
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

	void Gb28181Producer::OnProcessVideoFrame(libmedia_codec::EncodedImage frame)
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
		GetStream()->AddVideoFrame(std::move(frame));

	}
	void Gb28181Producer::OnProcessAudioFrame(rtc::CopyOnWriteBuffer frame, int64_t  pts)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "";
#if 0
		static FILE * out_file_ptr = fopen("ps_audio.aac", "wb+");
		if (out_file_ptr)
		{
			fwrite(frame.data(), 1, frame.size(), out_file_ptr);
			fflush(out_file_ptr);
		}
		//国标暂时不处理音频
		GetStream()->AddAudioFrame(std::move(frame), pts);
#endif // 0
		
	}
	 
}