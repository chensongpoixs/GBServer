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
#include "libmedia_transfer_protocol/rtp_video_frame_assembler.h"

#include "server/stream.h"
#include "server/session.h"
#include "producer/crtsp_producer.h"
#include "rtc_base/time_utils.h"

namespace gb_media_server {


 
	RtspProducer::RtspProducer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		:  Producer(  stream, s)
		//, rtp_video_frame_assembler_(std::make_unique<libmedia_transfer_protocol::RtpVideoFrameAssembler>(
	//		libmedia_transfer_protocol::RtpVideoFrameAssembler::kH264))
		, recv_buffer_(new uint8_t[kRecvBufferSize])
		, recv_buffer_size_(0)
		, video_ssrc_(0)
		, audio_ssrc_(0)
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


	 


		
	
	}
	RtspProducer::~RtspProducer()
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		if (recv_buffer_)
		{
			delete[] recv_buffer_;
			recv_buffer_ = nullptr;
		}
	}

	void RtspProducer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
	{
		// 将新数据追加到接收缓冲区
		if (recv_buffer_size_ + buffer1.size() > kRecvBufferSize)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "recv buffer overflow, reset buffer";
			recv_buffer_size_ = 0;
		}

		memcpy(recv_buffer_ + recv_buffer_size_, buffer1.data(), buffer1.size());
		recv_buffer_size_ += buffer1.size();

		int32_t parse_size = 0;

		// 解析 RTSP interleaved frames (TCP mode)
		while (recv_buffer_size_ - parse_size >= 4)
		{
			// 检查是否是 RTSP interleaved frame (magic byte '$')
			if (recv_buffer_[parse_size] != '$')
			{
				// 可能是 RTSP 协议消息，跳过
				GBMEDIASERVER_LOG(LS_WARNING) << "not RTSP interleaved frame, skip";
				parse_size = recv_buffer_size_;
				break;
			}

			// 读取 magic header
			RtspMagic magic;
			magic.magic_ = recv_buffer_[parse_size];
			magic.channel_ = recv_buffer_[parse_size + 1];
			magic.length_ = libmedia_transfer_protocol::ByteReader<uint16_t>::ReadBigEndian(
				&recv_buffer_[parse_size + 2]);

			// 检查是否有完整的数据包
			if (recv_buffer_size_ - parse_size < (4 + magic.length_))
			{
				// 数据不完整，等待更多数据
				break;
			}

			parse_size += 4; // 跳过 magic header

			// 根据 channel 判断是 RTP 还是 RTCP
			if (magic.channel_ == 0 || magic.channel_ == 1)
			{
				// RTP packet (channel 0) or RTCP packet (channel 1)
				rtc::ArrayView<uint8_t> packet_data(recv_buffer_ + parse_size, magic.length_);

				if (libmedia_transfer_protocol::IsRtpPacket(packet_data))
				{
					libmedia_transfer_protocol::RtpPacketReceived rtp_packet;
					if (rtp_packet.Parse(recv_buffer_ + parse_size, magic.length_))
					{
						ProcessRtpPacket(rtp_packet);
						if (video_ssrc_ == 0)
						{
							video_ssrc_ = rtp_packet.Ssrc();
						}
					}
					else
					{
						GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed";
					}
				}
				else if (libmedia_transfer_protocol::IsRtcpPacket(packet_data))
				{
					ProcessRtcpPacket(packet_data);
				}
				else
				{
					GBMEDIASERVER_LOG(LS_WARNING) << "unknown packet type, channel:" << (int)magic.channel_;
				}
			}
			else
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "unknown RTSP channel:" << (int)magic.channel_;
			}

			parse_size += magic.length_;
		}

		// 移动未处理的数据到缓冲区开头
		if (recv_buffer_size_ - parse_size > 0)
		{
			memmove(recv_buffer_, recv_buffer_ + parse_size, recv_buffer_size_ - parse_size);
			recv_buffer_size_ -= parse_size;
		}
		else
		{
			recv_buffer_size_ = 0;
		}
	}

	void RtspProducer::ProcessRtpPacket(const libmedia_transfer_protocol::RtpPacketReceived& rtp_packet)
	{
		// 使用 RtpVideoFrameAssembler 组装视频帧
		#if 0
		auto frames = rtp_video_frame_assembler_->InsertPacket(rtp_packet);

		for (auto& frame : frames)
		{
			if (frame && frame->size() > 0)
			{
				// 转换为 EncodedImage
				libmedia_codec::EncodedImage encoded_image;
				encoded_image.SetEncodedData(
					libmedia_codec::EncodedImageBuffer::Create(frame->data(), frame->size()));
				encoded_image.SetTimestamp(frame->Timestamp());
				encoded_image._frameType = frame->FrameType();
				encoded_image._encodedWidth = frame->_encodedWidth;
				encoded_image._encodedHeight = frame->_encodedHeight;

				// 推送到 Stream
				GetStream()->AddVideoFrame(std::move(encoded_image));
			}
		}
		#endif // rtp_video_frame_assembler_
	}

	void RtspProducer::ProcessRtcpPacket(const rtc::ArrayView<uint8_t>& data)
	{
		libmedia_transfer_protocol::rtcp::CommonHeader rtcp_header;
		if (rtcp_header.Parse(data.data(), data.size()))
		{
			// RTCP 包处理（可以用于统计、同步等）
			GBMEDIASERVER_LOG(LS_VERBOSE) << "received RTCP packet, type:" << (int)rtcp_header.type();
		}
		else
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "rtcp parse failed";
		}
	}
}

  