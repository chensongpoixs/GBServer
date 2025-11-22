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
				   date:  2025-11-03

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

#include "share/rtc_interface.h"

#include <random>
#include "consumer/rtc_consumer.h"
#include "server/session.h"
#include "server/stream.h"
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
#include "libmedia_transfer_protocol/librtc/rtc_errors.h"
#include "gb_media_server_log.h"
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
#include "libmedia_transfer_protocol/librtcp/twcc_context.h"
#include "libmedia_transfer_protocol/librtcp/rtcp.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_feedback.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_context.h"
#include "rtc_base/string_utils.h"
#include "rtc_base/string_encode.h"
#include "share/rtc_interface.h"

namespace gb_media_server
{
	void RtcInterface::OnSctpAssociationConnecting(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	void RtcInterface::OnSctpAssociationConnected(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	void RtcInterface::OnSctpAssociationFailed(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)   
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	void RtcInterface::OnSctpAssociationClosed(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)   
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	void RtcInterface::OnSctpAssociationSendData(
	libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
	const uint8_t* data, size_t len)  
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		dtls_.SendApplicationData(data, len);
	}
	void RtcInterface::OnSctpAssociationMessageReceived(
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

#if 0 // test echo hello 
		if (sctp_) {
			// 回显数据  [AUTO-TRANSLATED:7868d3a4]
			// Echo data
			sctp_->SendSctpMessage(params, ppid, msg, len);
		}
#endif //
	}


}


