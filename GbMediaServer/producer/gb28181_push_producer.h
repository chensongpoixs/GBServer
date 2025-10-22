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


#ifndef _C_GB_MEDIA_SERVER_GB28181_PUSH_Producer_H_
#define _C_GB_MEDIA_SERVER_GB28181_PUSH_Producer_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
 
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h"
#include "producer/gb28181_push_producer.h"
#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 


#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/libmpeg/mpeg_decoder.h"
#include "producer/producer.h"



namespace gb_media_server {


	class Gb28181PushProducer : public Producer, public sigslot::has_slots<>
	{
	public:
		explicit Gb28181PushProducer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		virtual ~Gb28181PushProducer();
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer)  ;


	public:
		void OnProcessVideoFrame(libmedia_codec::EncodedImage frame);
		void OnProcessAudioFrame( rtc::CopyOnWriteBuffer frame);
	private:


		std::unique_ptr< libmedia_transfer_protocol::libmpeg::MpegDecoder>    mpeg_decoder_;


		//rtc::Buffer recv_buffer_;
		   uint8_t  * recv_buffer_;
		int32_t recv_buffer_size_;
	};
}


#endif // 