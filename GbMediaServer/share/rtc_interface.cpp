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
#include "libmedia_transfer_protocol/librtc/SctpAssociation.h"

namespace gb_media_server
{


 

#if 0

	void RtcInterface::OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO) << "DTLS connecting";
	}

	void RtcInterface::OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
		libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
		uint8_t* srtpLocalKey,
		size_t srtpLocalKeyLen,
		uint8_t* srtpRemoteKey,
		size_t srtpRemoteKeyLen,
		std::string& remote_cert)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "DTLS connected";



		delete srtp_send_session_;
		srtp_send_session_ = nullptr;
		delete srtp_recv_session_;
		srtp_recv_session_ = nullptr;


		try
		{
			srtp_send_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::OUTBOUND, srtpCryptoSuite, srtpLocalKey, srtpLocalKeyLen);
		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR) << "error creating SRTP sending session: " << error.what();
		}
		try
		{
			srtp_recv_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::INBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);

		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR) << "error creating SRTP receiving session: %s", error.what();

			delete srtp_send_session_;
			srtp_send_session_ = nullptr;
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "dtls handshake done.";
		dtls_done_ = true;
		//srtp_session_.Init(dtls_.RecvKey(), dtls_.SendKey());
		// return;
		 // 完成验证后进行发送

		StartCapture();
	}
	void RtcInterface::OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "dtls send size:" << len;

		rtc::Buffer buffer(data, len);
		GbMediaService::GetInstance().GetRtcServer()->SendPacketTo(std::move(buffer), rtc_remote_address_, rtc::PacketOptions());
	}
	//void OnDtlsHandshakeDone(libmedia_transfer_protocol::libssl::Dtls *dtls);
	void RtcInterface::OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS remotely closed";
		dtls_done_ = false;

		StopCapture();

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().RemoveConsumer(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);
			});
		// 
	}
	void RtcInterface::OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS failed";
		dtls_done_ = false;

		StopCapture();

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().RemoveConsumer(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);

			});
		// 
	}
	void RtcInterface::OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len)
	{
		// Pass it to the parent transport.
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS application data recice data ";
		if (sctp_)
		{
			//sctp_
		}
	}
  
#endif // 0



	RtcInterface::RtcInterface()
		: dtls_(RtcService::GetInstance().GetTaskQueueFactory())
		//, rtp_header_extension_map_() 
		, srtp_send_session_(nullptr)
		, srtp_recv_session_(nullptr)
		, rtp_header_()
		, extension_manager_()
		, twcc_context_()
		, sctp_(nullptr)
	{
		local_ufrag_ = GetUFrag(8);
		local_passwd_ = GetUFrag(32);
		sdp_.SetLocalUFrag(local_ufrag_);
		sdp_.SetLocalPasswd(local_passwd_);
		extension_manager_.Register<libmedia_transfer_protocol::AbsoluteSendTime>(libmedia_transfer_protocol::kRtpExtensionAbsoluteSendTime);
		extension_manager_.Register<libmedia_transfer_protocol::TransportSequenceNumber>(libmedia_transfer_protocol::kRtpExtensionTransportSequenceNumber);
		// feedback  
		twcc_context_.setOnSendTwccCB([this](uint32_t ssrc, std::string fci) { onSendTwcc(ssrc, fci); });



		// sctp 

		sctp_ = std::make_shared<libmedia_transfer_protocol::librtc::SctpAssociationImp>(GbMediaService::GetInstance().worker_thread(), this, 128, 128, 262144, true);
		sctp_->TransportConnected();

	}
	RtcInterface::~RtcInterface() {}

	std::string RtcInterface::GetUFrag(int size) {
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
	uint32_t RtcInterface::GetSsrc(int size)
	{
		static std::mt19937 mt{ std::random_device{}() };
		static std::uniform_int_distribution<> rand(10000000, 99999999);

		return rand(mt);
	}
	void  RtcInterface::SetRtcRemoteAddress(const rtc::SocketAddress& addr)
	{
		rtc_remote_address_ = addr;
	}


	bool RtcInterface::SendSrtpRtp(uint8_t* data, size_t  size)
	{
		if (!dtls_done_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "dtls done failed !!!";
			return false;

		}
		if (!srtp_send_session_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "srtp_send_session_ == null failed !!!";
			return false;
		}
		//const uint8_t* data = single_packet->data();
		//size_t   len = single_packet->size();
		if (!srtp_send_session_->EncryptRtp((const uint8_t **)&data, &size))
		{
			return false;
		}


		GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, size),
			rtc_remote_address_, rtc::PacketOptions());


		return true;
	}
	bool RtcInterface::SendSrtpRtcp(uint8_t* data, size_t size)
	{

		if (!dtls_done_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "dtls done failed !!!";
			return false;

		}
		if (!srtp_send_session_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "srtp_send_session_ == null failed !!!";
			return false;
		}
		if (!srtp_send_session_->EncryptRtcp((const uint8_t **)&data, &size))
		{
			return false;
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "hex:" << rtc::hex_encode((const char*)data, size);

		GbMediaService::GetInstance().GetRtcServer()->SendRtcpPacketTo(rtc::CopyOnWriteBuffer(data, size),
			rtc_remote_address_, rtc::PacketOptions());
		return true;
	}

	void RtcInterface::SendDatachannel(uint16_t streamId, uint32_t ppid, const char* msg, size_t len)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);

		if (sctp_) {
			libmedia_transfer_protocol::librtc::SctpStreamParameters params;
			params.streamId = streamId;
			sctp_->SendSctpMessage(params, ppid, (uint8_t*)msg, len);
		}
	}

	void RtcInterface::onSendTwcc(uint32_t ssrc, const std::string& twcc_fci)
	{
		auto rtcp = libmedia_transfer_protocol::librtcp:: RtcpFB::create(libmedia_transfer_protocol::librtcp::RTPFBType::RTCP_RTPFB_TWCC , twcc_fci.data(), twcc_fci.size());
		rtcp->ssrc = htonl(0);
		rtcp->ssrc_media = htonl(ssrc);
		GBMEDIASERVER_LOG(LS_INFO) << "send twcc -- packet size:" << rtcp->getSize();
		SendSrtpRtcp((uint8_t*)rtcp.get(), rtcp->getSize());


		//SendSrtpRtcp();

		//std::unique_ptr< libmedia_transfer_protocol::rtcp::Pli> pli = std::make_unique< libmedia_transfer_protocol::rtcp::Pli>();
		//pli->SetSenderSsrc(sdp_.VideoSsrc());
		//pli->SetMediaSsrc(sdp_.VideoSsrc());

		//libmedia_transfer_protocol::rtcp::CompoundPacket compound;               // Builds a compound RTCP packet with
		//compound.Append(std::move(pli));                  // a receiver report, report block
	   // compound.Append(&fir);                 // and fir message.
		//rtc::Buffer packet = compound.Build();
		//SendSrtpRtcp(packet.data(), packet.size());
	}
}