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
				   date:  2025-10-13



 ******************************************************************************/
 
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

namespace gb_media_server
{
	 
	 void RtcConsumer::OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "DTLS connecting" ;
	}
	void RtcConsumer::OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
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
			GBMEDIASERVER_LOG_T_F(LS_ERROR)<< "error creating SRTP sending session: " <<  error.what();
		}
		try
		{
			srtp_recv_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::INBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);

			// Notify the Node WebRtcTransport.
			//json data = json::object();
			//
			//data["dtlsState"] = "connected";
			//data["dtlsRemoteCert"] = remoteCert;
			//
			//Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);
			//DEBUG_EX_LOG("data = %s", data.dump().c_str());
			//// Tell the parent class.
			//RTC::Transport::Connected();
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

#if TEST_RTC_PLAY
		if (capture_type_)
		{
			x264_encoder_ = std::make_unique<libmedia_codec::X264Encoder>();
			x264_encoder_->SignalVideoEncodedImage.connect(this, &RtcConsumer::SendVideoEncode);
			x264_encoder_->Start();
			video_encoder_thread_ = rtc::Thread::Create();
			video_encoder_thread_->SetName("video_encoder_thread", NULL);
			video_encoder_thread_->Start();

			capturer_track_source_ = libcross_platform_collection_render::CapturerTrackSource::Create(false);
			capturer_track_source_->set_catprue_callback(x264_encoder_.get(), video_encoder_thread_.get());
			capturer_track_source_->StartCapture();
		}

#endif // 1
	}
	void RtcConsumer::OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t *data, size_t len)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "dtls send size:" << len;
		 
		rtc::Buffer buffer(data, len);
		GbMediaService::GetInstance().GetRtcServer()->SendPacketTo(std::move(buffer), remote_address_, rtc::PacketOptions());
	}
	//void OnDtlsHandshakeDone(libmedia_transfer_protocol::libssl::Dtls *dtls);
	void RtcConsumer::OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls *dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS remotely closed";
		dtls_done_ = false;
#if TEST_RTC_PLAY
		if (capture_type_)
		{
			if (video_encoder_thread_)
			{
				video_encoder_thread_->Stop();
			}
			if (x264_encoder_)
			{
				//	x264_encoder_->SetSendFrame(nullptr);
				x264_encoder_->Stop();
			}
			if (capturer_track_source_)
			{
				//	capturer_track_source_->set_catprue_callback(nullptr, nullptr);
				capturer_track_source_->Stop();
			}
		}
		// GetSession()->CloseUser()
#endif //

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().RemoveConsumer(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);
		});
		// 
	}
	void RtcConsumer::OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls *dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS failed";
		dtls_done_ = false;
#if TEST_RTC_PLAY
		if (capture_type_)
		{
			if (video_encoder_thread_)
			{
				video_encoder_thread_->Stop();
			}
			if (x264_encoder_)
			{
				//	x264_encoder_->SetSendFrame(nullptr);
				x264_encoder_->Stop();
			}
			if (capturer_track_source_)
			{
				//	capturer_track_source_->set_catprue_callback(nullptr, nullptr);
				capturer_track_source_->Stop();
			}
		}
		// GetSession()->CloseUser()
#endif //

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().RemoveConsumer(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);
			
		});
		// 
	}
	void RtcConsumer::OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls *dtls, const uint8_t* data, size_t len)
	{
		// Pass it to the parent transport.
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS application data recice data ";
	}


	


}