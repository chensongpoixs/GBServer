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

#include "utils/yaml_config.h"
#include "yaml-cpp/yaml.h" 
#include "gb_media_server_log.h"
namespace  gb_media_server
{
	YamlConfig::YamlConfig()
		: http_server_config_()
		, rtc_server_config_()
		, rtp_port_config_()
	{
	}
	YamlConfig::~YamlConfig()
	{
	}
	bool YamlConfig::LoadFile(const char* file)
	{
		try {
			// 从文件加载YAML
			YAML::Node node = YAML::LoadFile(file);

			if (node["http"]) {
				http_server_config_.port= 	node["http"]["port"].as<uint16_t>();
			
				GBMEDIASERVER_LOG(LS_INFO) << "http server config \n port:" << http_server_config_.port;
			}

			if (node["rtc"]) {
				rtc_server_config_.ips = node["rtc"]["ips"].as<std::vector<std::string>>();
				rtc_server_config_.udp_port = node["rtc"]["udp"]["port"].as<uint16_t>();
				rtc_server_config_.tcp_port = node["rtc"]["tcp"]["port"].as<uint16_t>();
				rtc_server_config_.cert_public_key = node["rtc"]["cert"] .as<std::string>();;
				rtc_server_config_.cert_private_key = node["rtc"]["key"].as<std::string>();;
			
				std::ostringstream cmd;
				cmd << " ips:[";
				for (const auto& ip : rtc_server_config_.ips)
				{
					cmd << ip;
				}
				cmd << "]";
				cmd << ", udp_port:" << rtc_server_config_.udp_port;
				cmd << ", tcp_port:" << rtc_server_config_.tcp_port;
				cmd << ", public key:" << rtc_server_config_.cert_public_key;
				cmd << ", private key:" << rtc_server_config_.cert_private_key;

				GBMEDIASERVER_LOG(LS_INFO) << " rtc server config : \n " << cmd.str();
			}
			if (node["rtp"])
			{
				rtp_port_config_.udp_min_port = node["rtp"]["udp"]["min_port"].as<uint16_t>();
				rtp_port_config_.udp_max_port = node["rtp"]["udp"]["max_port"].as<uint16_t>();
				rtp_port_config_.tcp_min_port = node["rtp"]["tcp"]["min_port"].as<uint16_t>();
				rtp_port_config_.tcp_max_port = node["rtp"]["tcp"]["max_port"].as<uint16_t>();
				GBMEDIASERVER_LOG(LS_INFO) << "rtp config info ：\n" << "udp --> [min_port:" << rtp_port_config_.udp_min_port
					<< "][max_port:" << rtp_port_config_.udp_max_port << "]\n"
					<< "tcp [min_port:" << rtp_port_config_.tcp_min_port << "][max_port:" << rtp_port_config_.tcp_max_port <<"]";

			}
		}
		catch (const YAML::Exception& e) {
			std::cerr << "YAML Error: " << e.what() << "\n";
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "yaml parse failed !!!   error: " << e.what();
			return false;
			//return 1;
		}
		return true;
	}
}
