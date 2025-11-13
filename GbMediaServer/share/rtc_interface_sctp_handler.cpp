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
			// »ØÏÔÊý¾Ý  [AUTO-TRANSLATED:7868d3a4]
			// Echo data
			sctp_->SendSctpMessage(params, ppid, msg, len);
		}
#endif //
	}


}


