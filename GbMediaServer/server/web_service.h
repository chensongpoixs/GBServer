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
#ifndef _C_WEB_SERVICE_H_
#define _C_WEB_SERVICE_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"

#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"



namespace  gb_media_server
{
	class WebService  : public sigslot::has_slots<>
	{
	public:
		WebService();
		~WebService();
	
	public:

		bool StartWebServer(const char *ip = "127.0.0.1", uint16_t port = 8001);

	public:
		void OnRequest(libmedia_transfer_protocol::libhttp::TcpSession *conn, const  std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> http_request, const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet);



	public:

		void HandlerRtcPlay();
	private:

		std::unique_ptr< libmedia_transfer_protocol::libhttp::HttpServer>  http_server_;
	};
}

#endif // _C_WEB_SERVICE_H_