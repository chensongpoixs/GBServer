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


#ifndef _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_
#define _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
#include "consumer/consumer.h"
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h"

#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"



#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/muxer/muxer.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "share/share_resource.h"
#include "libmedia_transfer_protocol/librtcp/twcc_context.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/nack.h"
#include <memory>
namespace gb_media_server {
 
	class RtcInterface  : public libmedia_transfer_protocol::librtc::SctpAssociation::Listener
	{
	public:
		explicit RtcInterface();
		virtual ~RtcInterface() ;

	public:
	   virtual 	bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp) = 0;
	   virtual const std::string& LocalUFrag() const = 0;
	   virtual const std::string& LocalPasswd() const = 0;
	   virtual const std::string& RemoteUFrag() const = 0;
	   virtual std::string BuildAnswerSdp() = 0;

		//开始DTLS选择客户端还是服务端挥手交换 
	   virtual void MayRunDtls() = 0;
	public:

		bool SendSrtpRtp(uint8_t* data, size_t  size);
		bool SendSrtpRtcp(uint8_t* data, size_t size);


		void SendDatachannel(uint16_t streamId, uint32_t ppid, const char* msg, size_t len);
		// 

		virtual void OnDtlsRecv(const uint8_t* buf, size_t size) = 0;
		virtual void OnSrtpRtp(  uint8_t* data, size_t size) = 0;
		virtual void OnSrtpRtcp(  uint8_t* data, size_t size) = 0;
	
		
	public:
		virtual void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		virtual void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert) = 0;
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len) = 0;

		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len) = 0;
	public:
		void onSendTwcc(uint32_t ssrc, const std::string& twcc_fci);




		void CreateDataChannel();


		void AddVideoPacket(std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend> rtp_packet);


		void RequestNack(const libmedia_transfer_protocol::rtcp::Nack& nack);
	public:

		// sctp inferface
		virtual void OnSctpAssociationConnecting(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationConnected(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationFailed(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationClosed(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		virtual void OnSctpAssociationSendData(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation, 
			const uint8_t* data, size_t len)  override;
		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;

	public:
		// rtc 特别增加的接口
		virtual  const rtc::SocketAddress& RtcRemoteAddress() const
		{
			return rtc_remote_address_;
		}
		virtual void  SetRtcRemoteAddress(const rtc::SocketAddress& addr);

	

	public:
		static std::string GetUFrag(int size);
		static uint32_t GetSsrc(int size);
	protected:


		std::string local_ufrag_;
		std::string local_passwd_;  //[12, 32]
		libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		//Dtls dtls_;

		//libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		libmedia_transfer_protocol::libssl::Dtls   dtls_;

		bool dtls_done_{ false };

		rtc::SocketAddress             rtc_remote_address_;
		libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_send_session_;
		libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_recv_session_;




		uint32_t      audio_seq_ = 100;
		uint32_t      video_seq_ = 100;
		uint32_t      video_rtx_seq_ = 100;
		//libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;
		libmedia_transfer_protocol::RTPHeader  rtp_header_;
		libmedia_transfer_protocol::RtpHeaderExtensionMap    extension_manager_;
		libmedia_transfer_protocol::librtcp::TwccContext     twcc_context_;

		//libmedia_transfer_protocol::librtc::SctpAssociationImp::Ptr   sctp_;
		std::shared_ptr< libmedia_transfer_protocol::librtc::SctpAssociationImp> sctp_;


		std::unordered_map<uint32_t, std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend>>   rtp_video_packets_;
	};
 
}
#endif // _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_