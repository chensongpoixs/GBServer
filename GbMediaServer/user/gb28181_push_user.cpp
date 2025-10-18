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
#include "user/gb28181_push_user.h"

#include "rtc_base/logging.h"
namespace gb_media_server {


 
	Gb28181PushUser::Gb28181PushUser(const std::shared_ptr<Connection>& ptr, 
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		:User(ptr, stream, s),
		mpeg_decoder_(nullptr){
		mpeg_decoder_ = std::make_unique<libmedia_transfer_protocol::libmpeg::MpegDecoder>();
		mpeg_decoder_->SignalRecvVideoFrame.connect(this, &Gb28181PushUser::OnProcessVideoFrame);
		mpeg_decoder_->SignalRecvAudioFrame.connect(this, &Gb28181PushUser::OnProcessAudioFrame);
		
	}
	Gb28181PushUser::~Gb28181PushUser()
	{
		if (mpeg_decoder_)
		{
			mpeg_decoder_->SignalRecvVideoFrame.disconnect_all();
			//mpeg_decoder_->SignalRecvAudioFrame.disconnect(this);
		}
	}
	void Gb28181PushUser::OnRecv(const rtc::CopyOnWriteBuffer&  buffer)
	{
		mpeg_decoder_->parse(buffer.data(), buffer.size());
		
	}

	void Gb28181PushUser::OnProcessVideoFrame(libmedia_codec::EncodedImage frame)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "";
	}
	void Gb28181PushUser::OnProcessAudioFrame(rtc::CopyOnWriteBuffer frame)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "";
	}
	 
}