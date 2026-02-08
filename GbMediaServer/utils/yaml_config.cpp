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

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。

 ******************************************************************************/

#include "utils/yaml_config.h"
#include "yaml-cpp/yaml.h" 
#include "gb_media_server_log.h"
namespace  gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief YamlConfig构造函数实现（YamlConfig Constructor Implementation）
	*  
	*  该构造函数用于初始化YamlConfig实例。它会初始化所有配置结构体为默认值。
	*  
	*  初始化流程：
	*  1. 初始化http_server_config_为默认值（ip="127.0.0.1", port=8001）
	*  2. 初始化rtc_server_config_为默认值（ips=["127.0.0.1"], udp_port=10001等）
	*  3. 初始化rtp_port_config_为默认值（udp_min_port=40000等）
	*  
	*  @note 构造函数是私有的，只能通过GetInstance方法获取实例（单例模式）
	*  @note 默认值在结构体定义中指定，构造函数只需调用默认构造函数
	*/
	YamlConfig::YamlConfig()
		: http_server_config_()   // 使用默认构造函数初始化HTTP服务器配置
		, rtc_server_config_()    // 使用默认构造函数初始化RTC服务器配置
		, rtp_port_config_()      // 使用默认构造函数初始化RTP端口配置
	{
		// 所有配置项都已通过成员初始化列表初始化为默认值
		// 实际配置将在LoadFile方法中从YAML文件加载
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief YamlConfig析构函数实现（YamlConfig Destructor Implementation）
	*  
	*  该析构函数用于清理YamlConfig实例。由于YamlConfig只包含简单的数据成员，
	*  不需要手动释放资源，析构函数为空。
	*  
	*  @note 析构函数会自动调用，不需要手动释放资源
	*  @note 配置结构体的析构由编译器自动处理
	*/
	YamlConfig::~YamlConfig()
	{
		// 无需手动清理资源
		// 所有成员变量都是值类型或标准库容器，会自动析构
	}
}
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief 从YAML配置文件加载配置（Load Configuration File）
	*  
	*  该方法用于从YAML格式的配置文件中加载服务器配置信息。
	*  配置文件包含HTTP服务器、RTC服务器和RTP端口范围等配置项。
	*  
	*  支持的配置项：
	*  1. HTTP服务器配置（http节点）：
	*     - port: HTTP服务器监听端口
	*  
	*  2. RTC服务器配置（rtc节点）：
	*     - ips: 服务器IP地址列表（支持多个IP）
	*     - udp.port: UDP监听端口
	*     - tcp.port: TCP监听端口
	*     - cert: DTLS证书公钥文件路径
	*     - key: DTLS证书私钥文件路径
	*  
	*  3. RTP端口配置（rtp节点）：
	*     - udp.min_port: UDP端口范围的最小值
	*     - udp.max_port: UDP端口范围的最大值
	*     - tcp.min_port: TCP端口范围的最小值
	*     - tcp.max_port: TCP端口范围的最大值
	*  
	*  处理流程：
	*  1. 使用yaml-cpp库加载YAML文件
	*  2. 解析http节点，提取HTTP服务器端口
	*  3. 解析rtc节点，提取RTC服务器IP、端口和证书路径
	*  4. 解析rtp节点，提取RTP端口范围
	*  5. 记录配置信息到日志
	*  6. 如果解析失败，捕获异常并返回false
	*  
	*  @param file 配置文件路径，可以是相对路径或绝对路径
	*  @return 如果加载成功返回true，否则返回false
	*  @note 配置文件必须是有效的YAML格式
	*  @note 如果配置项缺失，会使用默认值或导致加载失败
	*  @note 加载失败时会记录错误日志
	*  
	*  YAML配置文件示例：
	*  @code
	*  http:
	*    port: 8080
	*  
	*  rtc:
	*    ips:
	*      - 192.168.1.100
	*      - 10.0.0.1
	*    udp:
	*      port: 9091
	*    tcp:
	*      port: 9092
	*    cert: /path/to/cert.pem
	*    key: /path/to/key.pem
	*  
	*  rtp:
	*    udp:
	*      min_port: 10000
	*      max_port: 20000
	*    tcp:
	*      min_port: 10000
	*      max_port: 20000
	*  @endcode
	*  
	*  使用示例：
	*  @code
	*  YamlConfig& config = YamlConfig::GetInstance();
	*  if (config.LoadFile("config.yaml")) {
	*      uint16_t http_port = config.GetHttpServerConfig().port;
	*      std::vector<std::string> ips = config.GetRtcServerConfig().ips;
	*  } else {
	*      // 处理加载失败
	*  }
	*  @endcode
	*/
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
