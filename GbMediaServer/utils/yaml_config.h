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


#ifndef _C_GB_MEDIA_SERVER_CONFIG_H_H_
#define _C_GB_MEDIA_SERVER_CONFIG_H_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"

#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"



namespace  gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief HTTP服务器配置结构体（HTTP Server Configuration）
	*  
	*  该结构体用于存储HTTP服务器的配置信息，包括监听IP和端口。
	*  HTTP服务器用于提供Web管理界面、API接口等功能。
	*  
	*  配置项说明：
	*  - ip: HTTP服务器监听的IP地址，默认为"127.0.0.1"（本地回环地址）
	*  - port: HTTP服务器监听的端口号，默认为8001
	*  
	*  @note 默认配置只监听本地地址，如需外部访问需修改为"0.0.0.0"
	*  @note 端口号范围为1-65535，建议使用1024以上的端口
	*  
	*  使用示例：
	*  @code
	*  HttpServerConfig config;
	*  config.ip = "0.0.0.0";  // 监听所有网卡
	*  config.port = 8080;     // 使用8080端口
	*  @endcode
	*/
	struct HttpServerConfig
	{
		std::string ip{ "127.0.0.1" };  // HTTP服务器监听IP地址，默认本地回环
		uint16_t port{8001};            // HTTP服务器监听端口，默认8001
	};
	
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief RTC服务器配置结构体（RTC Server Configuration）
	*  
	*  该结构体用于存储RTC（WebRTC）服务器的配置信息，包括监听IP、端口和DTLS证书。
	*  RTC服务器用于处理WebRTC连接，支持UDP和TCP两种传输协议。
	*  
	*  配置项说明：
	*  - ips: RTC服务器监听的IP地址列表，支持多个IP（多网卡场景），默认为["127.0.0.1"]
	*  - udp_port: UDP监听端口，用于接收STUN、DTLS、RTP、RTCP数据包，默认为10001
	*  - tcp_port: TCP监听端口，用于TCP传输模式（较少使用），默认为10001
	*  - cert_public_key: DTLS证书公钥文件路径，用于DTLS握手和加密
	*  - cert_private_key: DTLS证书私钥文件路径，用于DTLS握手和加密
	*  
	*  DTLS证书说明：
	*  - DTLS（Datagram Transport Layer Security）用于WebRTC的密钥交换
	*  - 证书可以是自签名证书或CA签发的证书
	*  - 证书格式通常为PEM格式
	*  
	*  @note 多IP配置用于多网卡服务器，每个IP都会创建独立的监听套接字
	*  @note UDP端口是必需的，TCP端口是可选的
	*  @note 证书文件路径必须是有效的文件路径，否则DTLS握手会失败
	*  
	*  使用示例：
	*  @code
	*  RtcServerConfig config;
	*  config.ips = {"192.168.1.100", "10.0.0.1"};  // 监听两个网卡
	*  config.udp_port = 9091;
	*  config.tcp_port = 9092;
	*  config.cert_public_key = "/path/to/cert.pem";
	*  config.cert_private_key = "/path/to/key.pem";
	*  @endcode
	*/
	struct RtcServerConfig
	{
		std::vector<std::string> ips{ "127.0.0.1" };  // RTC服务器监听IP列表，支持多网卡
		uint16_t        udp_port{10001};              // UDP监听端口，用于WebRTC数据传输
		uint16_t        tcp_port{10001};              // TCP监听端口，用于TCP传输模式
		std::string     cert_public_key{""};          // DTLS证书公钥文件路径
		std::string     cert_private_key{ "" };       // DTLS证书私钥文件路径
	};
	
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief RTP端口配置结构体（RTP Port Configuration）
	*  
	*  该结构体用于存储RTP端口范围配置，用于动态分配RTP/RTCP端口。
	*  在处理RTSP、GB28181等协议时，需要为每个会话分配独立的RTP/RTCP端口对。
	*  
	*  配置项说明：
	*  - udp_min_port: UDP端口范围的最小值，默认为40000
	*  - udp_max_port: UDP端口范围的最大值，默认为50000
	*  - tcp_min_port: TCP端口范围的最小值，默认为40000
	*  - tcp_max_port: TCP端口范围的最大值，默认为50000
	*  
	*  端口分配说明：
	*  - RTP使用偶数端口，RTCP使用相邻的奇数端口（RTP端口+1）
	*  - 端口范围应足够大，以支持并发会话数量
	*  - 端口范围不应与其他服务冲突
	*  
	*  @note 端口范围应为偶数，以便正确分配RTP/RTCP端口对
	*  @note 端口范围越大，支持的并发会话数越多
	*  @note 建议使用40000-50000范围，避免与常用端口冲突
	*  
	*  使用示例：
	*  @code
	*  RtpPortConfig config;
	*  config.udp_min_port = 10000;
	*  config.udp_max_port = 20000;
	*  // 支持 (20000-10000)/2 = 5000 个并发会话
	*  @endcode
	*/
	struct RtpPortConfig
	{
		uint16_t   udp_min_port{40000};  // UDP端口范围最小值
		uint16_t   udp_max_port{50000};  // UDP端口范围最大值
		uint16_t   tcp_min_port{40000};  // TCP端口范围最小值
		uint16_t   tcp_max_port{50000};  // TCP端口范围最大值
	};
	
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief YAML配置管理类（YAML Configuration Manager）
	*  
	*  YamlConfig是GBMediaServer流媒体服务器的配置管理类，采用单例模式。
	*  它负责从YAML格式的配置文件中加载服务器配置，并提供配置访问接口。
	*  
	*  主要功能：
	*  1. 从YAML文件加载配置（LoadFile）
	*  2. 解析HTTP服务器配置
	*  3. 解析RTC服务器配置
	*  4. 解析RTP端口范围配置
	*  5. 提供配置访问接口
	*  
	*  配置文件格式：
	*  - 使用YAML格式，易于阅读和编辑
	*  - 支持嵌套结构，便于组织配置项
	*  - 支持列表、字符串、数字等数据类型
	*  
	*  工作流程：
	*  1. 服务器启动时调用LoadFile加载配置文件
	*  2. 使用yaml-cpp库解析YAML文件
	*  3. 提取各个配置项并存储到对应的结构体中
	*  4. 其他模块通过GetXxxConfig方法获取配置
	*  
	*  @note YamlConfig采用单例模式，确保全局只有一个配置实例
	*  @note 配置文件加载失败会返回false，应用应检查返回值
	*  @note 配置加载后不会自动重新加载，需要重启服务器才能生效
	*  
	*  使用示例：
	*  @code
	*  // 加载配置文件
	*  YamlConfig& config = YamlConfig::GetInstance();
	*  if (!config.LoadFile("config.yaml")) {
	*      // 处理加载失败
	*      return -1;
	*  }
	*  
	*  // 获取HTTP服务器配置
	*  const HttpServerConfig& http_config = config.GetHttpServerConfig();
	*  uint16_t http_port = http_config.port;
	*  
	*  // 获取RTC服务器配置
	*  const RtcServerConfig& rtc_config = config.GetRtcServerConfig();
	*  std::vector<std::string> ips = rtc_config.ips;
	*  @endcode
	*/
	class YamlConfig
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化YamlConfig实例。它会初始化所有配置结构体为默认值。
		*  
		*  @note 构造函数是私有的，只能通过GetInstance方法获取实例（单例模式）
		*/
		explicit YamlConfig();
		
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理YamlConfig实例。
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual ~YamlConfig();

	public:
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 获取单例实例（Get Instance）
		*  
		*  该方法用于获取YamlConfig的单例实例。采用线程安全的单例模式实现，
		*  确保整个系统中只有一个配置实例。
		*  
		*  @return 返回YamlConfig单例实例的引用
		*  @note 该方法线程安全，使用C++11的静态局部变量实现单例模式
		*  @note 单例实例在第一次调用时创建，程序结束时自动销毁
		*/
		static YamlConfig& GetInstance()
		{
			static YamlConfig instance;
			return instance;
		}
		
	public:
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 从YAML配置文件加载配置（Load Configuration File）
		*  
		*  该方法用于从YAML格式的配置文件中加载服务器配置信息。
		*  
		*  @param file 配置文件路径，可以是相对路径或绝对路径
		*  @return 如果加载成功返回true，否则返回false
		*  @note 配置文件必须是有效的YAML格式
		*  @note 加载失败时会记录错误日志
		*/
		bool LoadFile(const char* file);

		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 获取HTTP服务器配置（Get HTTP Server Configuration）
		*  
		*  该方法用于获取HTTP服务器的配置信息。
		*  
		*  @return 返回HttpServerConfig结构体的常量引用
		*  @note 返回的是常量引用，不能修改配置
		*/
		const HttpServerConfig& GetHttpServerConfig() const { return http_server_config_; }
		
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 获取RTC服务器配置（Get RTC Server Configuration）
		*  
		*  该方法用于获取RTC服务器的配置信息。
		*  
		*  @return 返回RtcServerConfig结构体的常量引用
		*  @note 返回的是常量引用，不能修改配置
		*/
		const RtcServerConfig& GetRtcServerConfig() const { return rtc_server_config_; }
		
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 获取RTP端口配置（Get RTP Port Configuration）
		*  
		*  该方法用于获取RTP端口范围的配置信息。
		*  
		*  @return 返回RtpPortConfig结构体的常量引用
		*  @note 返回的是常量引用，不能修改配置
		*/
		const RtpPortConfig& GetRtpPortConfig() const { return rtp_port_config_; }

	public:
		HttpServerConfig     http_server_config_;   // HTTP服务器配置
		RtcServerConfig      rtc_server_config_;    // RTC服务器配置
		RtpPortConfig       rtp_port_config_;       // RTP端口配置
		
	 };
}


#endif // _C_GB_MEDIA_SERVER_STRING_UTILS_H_