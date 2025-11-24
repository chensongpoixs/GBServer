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
 /***********************************************************************************************
 created: 		2025-04-29

 author:			chensong

 purpose:		RtmpConsumer
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
 ************************************************************************************************/
#include "consumer/crtsp_consumer.h" 
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format_h264.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "gb_media_server_log.h"
#include "rtc_base/time_utils.h"
#include "modules/video_coding/codecs/h264/include/h264_globals.h"

namespace gb_media_server
{
	RtspConsumer::RtspConsumer(libmedia_transfer_protocol::libnetwork::Connection*    connection,
		std::shared_ptr<Stream>& stream, std::shared_ptr<Session>& s)
		: Consumer(stream, s)
		, connection_(connection)
		, rtp_header_extension_map_(std::make_unique<libmedia_transfer_protocol::RtpHeaderExtensionMap>())
		, video_ssrc_(0x12345678)  // 默认 SSRC
		, video_sequence_number_(0)
		, video_rtp_timestamp_(0)
		, video_payload_type_(96)  // H264 payload type
	{
		// 初始化 RTP header extension map
		rtp_header_extension_map_->RegisterByType(1, libmedia_transfer_protocol::kRtpExtensionTransmissionTimeOffset);
		rtp_header_extension_map_->RegisterByType(2, libmedia_transfer_protocol::kRtpExtensionAbsoluteSendTime);
	}

	RtspConsumer::~RtspConsumer()
	{ 
	}

	void RtspConsumer::OnVideoFrame(const libmedia_codec::EncodedImage & frame)
	{
		if (frame.size() <= 0 || !connection_)
		{
			return;
		}

		PacketizeH264Frame(frame);
	}

	void RtspConsumer::OnAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	{
		// TODO: 实现音频 RTP 封装
		GBMEDIASERVER_LOG(LS_VERBOSE) << "audio frame received, size:" << frame.size() << ", pts:" << pts;
	}

	void RtspConsumer::PacketizeH264Frame(const libmedia_codec::EncodedImage& frame)
	{
		// 创建 RTP packetizer
		libmedia_transfer_protocol::RtpPacketizer::PayloadSizeLimits limits;
		limits.max_payload_len = 1200;  // 最大 RTP payload 大小
		limits.first_packet_reduction_len = 0;
		limits.last_packet_reduction_len = 0;
		limits.single_packet_reduction_len = 0;

		rtc::ArrayView<const uint8_t> payload(frame.data(), frame.size());

		// 创建 H264 packetizer
		auto packetizer = std::make_unique<libmedia_transfer_protocol::RtpPacketizerH264>(
			payload, limits, webrtc::H264PacketizationMode::NonInterleaved);

		// 创建 RTP header
		libmedia_transfer_protocol::RTPVideoHeader rtp_video_header;
		rtp_video_header.codec = libmedia_codec::kVideoCodecH264;
		rtp_video_header.frame_type = frame._frameType;
		rtp_video_header.width = frame._encodedWidth;
		rtp_video_header.height = frame._encodedHeight;
		auto& h264_header = rtp_video_header.video_type_header.emplace<webrtc::RTPVideoHeaderH264>();
		h264_header.packetization_mode = webrtc::H264PacketizationMode::NonInterleaved;

		// 更新 RTP timestamp (90kHz clock)
		video_rtp_timestamp_ = static_cast<uint32_t>(frame.Timestamp() * 90);  // 假设 timestamp 是毫秒

		// 封装每个 RTP 包
		while (packetizer->NumPackets() > 0)
		{
			auto rtp_packet = std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(
				rtp_header_extension_map_.get());

			// 设置 RTP header
			rtp_packet->SetPayloadType(video_payload_type_);
			rtp_packet->SetSequenceNumber(video_sequence_number_++);
			rtp_packet->SetTimestamp(video_rtp_timestamp_);
			rtp_packet->SetSsrc(video_ssrc_);
			rtp_packet->set_packet_type(libmedia_transfer_protocol::RtpPacketMediaType::kVideo);
			rtp_packet->set_capture_time_ms(frame.Timestamp());

			// 生成 RTP payload
			if (!packetizer->NextPacket(rtp_packet.get()))
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "failed to generate RTP packet";
				break;
			}

			// 设置 marker bit (最后一个包)
			if (packetizer->NumPackets() == 0)
			{
				rtp_packet->SetMarker(true);
			}

			// 通过 RTSP interleaved 模式发送
			SendRtpPacketInterleaved(*rtp_packet, kRtpChannel);
		}
	}

	void RtspConsumer::SendRtpPacketInterleaved(const libmedia_transfer_protocol::RtpPacketToSend& rtp_packet, uint8_t channel)
	{
		if (!connection_)
		{
			return;
		}

		// RTSP interleaved frame format: $<channel><length><data>
		// $: 1 byte
		// channel: 1 byte
		// length: 2 bytes (big-endian)
		// data: RTP packet

		size_t rtp_packet_size = rtp_packet.size();
		size_t interleaved_frame_size = 4 + rtp_packet_size;  // 4 bytes header + RTP packet

		rtc::CopyOnWriteBuffer buffer(interleaved_frame_size);
		uint8_t* data = buffer.MutableData();

		// 写入 magic byte '$'
		data[0] = '$';

		// 写入 channel
		data[1] = channel;

		// 写入 length (big-endian)
		libmedia_transfer_protocol::ByteWriter<uint16_t>::WriteBigEndian(
			&data[2], static_cast<uint16_t>(rtp_packet_size));

		// 写入 RTP packet
		memcpy(&data[4], rtp_packet.data(), rtp_packet_size);

		// 发送数据
		//connection_->Send(buffer);
	}
}
