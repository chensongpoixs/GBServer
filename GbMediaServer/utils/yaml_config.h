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


#ifndef _C_GB_MEDIA_SERVER_CONFIG_H_H_
#define _C_GB_MEDIA_SERVER_CONFIG_H_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"

#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"



namespace  gb_media_server
{


	struct HttpServerConfig
	{
		std::string ip{ "127.0.0.1" };
		uint16_t port{8001};
	};
	struct RtcServerConfig
	{
		std::vector<std::string> ips{ "127.0.0.1" };
		uint16_t        udp_port{10001};
		uint16_t        tcp_port{10001};
		// ÷§ È
		std::string     cert_public_key{""};
		std::string     cert_private_key{ "" };
	};
	struct RtpPortConfig
	{
		uint16_t   udp_min_port{40000};
		uint16_t   udp_max_port{50000};
		uint16_t   tcp_min_port{40000};
		uint16_t   tcp_max_port{50000};
	};
	class YamlConfig
	{
	public:
		explicit YamlConfig();
		virtual ~YamlConfig();



	public:

		static YamlConfig& GetInstance()
		{
			static YamlConfig instance;
			return instance;
		}
	public:
		bool LoadFile(const char* file);




		const HttpServerConfig& GetHttpServerConfig() const { return http_server_config_; }
		const RtcServerConfig& GetRtcServerConfig() const { return rtc_server_config_; }
		const RtpPortConfig& GetRtpPortConfig() const { return rtp_port_config_; }

	public:
		

		HttpServerConfig     http_server_config_;
		RtcServerConfig      rtc_server_config_;


		RtpPortConfig       rtp_port_config_;
		
	 };
}


#endif // _C_GB_MEDIA_SERVER_STRING_UTILS_H_