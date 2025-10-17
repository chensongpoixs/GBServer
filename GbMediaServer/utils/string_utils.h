/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-17



 ******************************************************************************/


#ifndef _C_GB_MEDIA_SERVER_STRING_UTILS_H_
#define _C_GB_MEDIA_SERVER_STRING_UTILS_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"

#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"



namespace  gb_media_server
{
	namespace string_utils 
	{

		size_t split(std::string source,
			char delimiter,
			std::vector<std::string>* fields);

		std::string  GetSessionNameFromUrl(const std::string &url);
	}
}


#endif // _C_GB_MEDIA_SERVER_STRING_UTILS_H_