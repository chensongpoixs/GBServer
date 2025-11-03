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


#ifndef _C_GB_MEDIA_SERVER_CAPUTRE_H_
#define _C_GB_MEDIA_SERVER_CAPUTRE_H_

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
#include "rtc_base/copy_on_write_buffer.h"

#include "libmedia_codec/encoded_image.h"
#include "rtc_base/socket_address.h"
 //#include "server/stream.h"
 //#include "server/session.h"
#include "share/share_resource.h"

#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 
#include "libmedia_transfer_protocol/muxer/muxer.h"
namespace gb_media_server {

	class Capture : public sigslot::has_slots<>
	{
	public:
		explicit Capture();
		virtual ~Capture();
	public:
		void SetCapture(bool value);


		void StartCapture();
		void StopCapture();

		// 本地采集的数据进行编码后进行发送的接口
		void   SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> f);
		void   SendAudioEncode(std::shared_ptr<libmedia_codec::AudioEncoder::EncodedInfoLeaf> f);

	protected:
#if TEST_RTC_PLAY

		bool										capture_type_;//采集桌面画面播放
		std::unique_ptr< rtc::Thread>        video_encoder_thread_;
		std::unique_ptr< libmedia_codec::X264Encoder>                          x264_encoder_;
		rtc::scoped_refptr<libcross_platform_collection_render::CapturerTrackSource>     capturer_track_source_;

#endif //
	};

}

#endif // 