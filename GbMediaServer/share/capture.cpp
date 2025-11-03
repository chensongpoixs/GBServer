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
#include "share/capture.h"


namespace gb_media_server
{
	Capture::Capture()
		: capture_type_(false)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}
	Capture::~Capture() {
		GBMEDIASERVER_LOG_T_F(LS_INFO);

		if (x264_encoder_)
		{
			x264_encoder_->SignalVideoEncodedImage.disconnect_all();
		}
#if TEST_RTC_PLAY
		{
			if (capture_type_)
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

#endif // 	
	}
 
	void Capture::SetCapture(bool value)
	{
		capture_type_ = value;
	}


	void Capture::StartCapture(){
#if TEST_RTC_PLAY
		if (capture_type_)
		{
			x264_encoder_ = std::make_unique<libmedia_codec::X264Encoder>();
			x264_encoder_->SignalVideoEncodedImage.connect(this, &Capture::SendVideoEncode);
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
	void Capture::StopCapture(){
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
	}

	// 本地采集的数据进行编码后进行发送的接口
	void   Capture::SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> f){}
	void   Capture::SendAudioEncode(std::shared_ptr<libmedia_codec::AudioEncoder::EncodedInfoLeaf> f){}
}
