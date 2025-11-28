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


#ifndef _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_
#define _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_

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
#include "share/share_resource.h"
#include "libmedia_transfer_protocol/librtcp/twcc_context.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/nack.h"
#include <memory>
#include <unordered_map>
#include <map>
namespace gb_media_server {
 
	/**
	*  @author chensong
	*  @date 2025-11-03
	*  @brief RTC接口基类（RTC Interface）
	*  
	*  RtcInterface是GBMediaServer流媒体服务器中所有RTC相关类的基类接口。
	*  它提供了WebRTC协议的核心功能，包括SDP处理、DTLS握手、SRTP加密、
	*  SCTP数据通道等。RtcProducer和RtcConsumer都继承自此类。
	*  
	*  WebRTC协议说明：
	*  - WebRTC（Web Real-Time Communication）是一种实时通信技术
	*  - WebRTC使用SDP（Session Description Protocol）进行信令交换
	*  - WebRTC使用DTLS（Datagram Transport Layer Security）进行密钥交换
	*  - WebRTC使用SRTP（Secure Real-time Transport Protocol）进行媒体加密传输
	*  - WebRTC使用SCTP（Stream Control Transmission Protocol）进行数据通道传输
	*  
	*  RtcInterface功能：
	*  1. SDP处理：解析Offer和生成Answer
	*  2. DTLS握手：建立安全连接，交换密钥
	*  3. SRTP加密：加密和解密RTP/RTCP包
	*  4. SCTP数据通道：支持非媒体数据传输
	*  5. RTP/RTCP处理：发送和接收媒体包
	*  6. TWCC（Transport-CC）：带宽估计和拥塞控制
	*  
	*  @note RtcInterface是抽象基类，不能直接实例化
	*  @note 子类必须实现所有纯虚函数
	*  @note 提供了完整的WebRTC协议栈实现
	*  
	*  继承层次：
	*  - RtcProducer: 继承自RtcInterface，用于接收WebRTC推流
	*  - RtcConsumer: 继承自RtcInterface，用于发送WebRTC拉流
	*  
	*  使用示例：
	*  @code
	*  // RtcInterface不能直接实例化，通过子类使用
	*  auto producer = std::make_shared<RtcProducer>(stream, session);
	*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
	*  @endcode
	*/
	class RtcInterface  : public libmedia_transfer_protocol::librtc::SctpAssociation::Listener
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化RtcInterface实例。它会初始化所有成员变量，
		*  包括SDP对象、DTLS对象、SRTP会话、SCTP关联等。
		*  
		*  初始化流程：
		*  1. 初始化本地用户名片段（ufrag）和密码（pwd）
		*  2. 初始化SDP对象
		*  3. 初始化DTLS对象，连接DTLS信号
		*  4. 初始化SRTP发送和接收会话（初始为空）
		*  5. 初始化RTP序列号和RTX序列号
		*  6. 初始化RTP头部和扩展管理器
		*  7. 初始化TWCC上下文
		*  8. 初始化SCTP关联
		*  
		*  @note 构造函数会生成随机的用户名片段和密码
		*  @note DTLS对象会在SDP处理时进一步配置
		*  @note SRTP会话会在DTLS握手成功后创建
		*  
		*  使用示例：
		*  @code
		*  // 通过子类构造函数隐式调用
		*  auto producer = std::make_shared<RtcProducer>(stream, session);
		*  @endcode
		*/
		explicit RtcInterface();

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtcInterface实例。它会释放所有相关资源，
		*  包括DTLS对象、SRTP会话、SCTP关联等。
		*  
		*  清理流程：
		*  1. 关闭DTLS连接（如果已连接）
		*  2. 释放SRTP发送和接收会话
		*  3. 关闭SCTP关联
		*  4. 清理所有资源
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual ~RtcInterface() ;

	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 处理SDP Offer（Process Offer SDP）
		*  
		*  该方法用于处理客户端发送的SDP Offer。它会解析SDP内容，提取音频/视频编解码器、
		*  ICE候选、DTLS指纹等信息，并准备生成SDP Answer。
		*  
		*  处理流程：
		*  1. 解析SDP Offer，提取会话信息
		*  2. 提取音频/视频媒体描述（m=行）
		*  3. 提取ICE候选（candidate）
		*  4. 提取DTLS指纹（fingerprint）
		*  5. 提取用户名片段（ufrag）和密码（pwd）
		*  6. 根据提取的信息准备生成Answer
		*  
		*  @param rtc_sdp_type SDP类型，kRtcSdpPush表示推流，kRtcSdpPlay表示拉流
		*  @param sdp SDP Offer的字符串内容
		*  @return 如果处理成功返回true，否则返回false
		*  @note 该方法会验证SDP格式和内容
		*  @note 处理成功后，可以通过BuildAnswerSdp()生成Answer
		*  @note 该方法是纯虚函数，子类必须实现
		*  
		*  使用示例：
		*  @code
		*  std::string sdp_offer = "..."; // SDP Offer内容
		*  if (rtc_interface->ProcessOfferSdp(libmedia_transfer_protocol::librtc::kRtcSdpPush, sdp_offer)) {
		*      std::string answer = rtc_interface->BuildAnswerSdp();
		*  }
		*  @endcode
		*/
	   virtual 	bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp) = 0;
	   
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 获取本地用户名片段（Get Local Username Fragment）
		*  
		*  该方法用于获取本地生成的ICE用户名片段（ufrag）。用户名片段用于ICE连接建立。
		*  
		*  @return 返回本地用户名片段的常量引用
		*  @note 用户名片段在构造函数中自动生成
		*  @note 该方法是纯虚函数，子类必须实现
		*/
	   virtual const std::string& LocalUFrag() const = 0;
	   
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 获取本地密码（Get Local Password）
		*  
		*  该方法用于获取本地生成的ICE密码（pwd）。密码用于ICE连接建立。
		*  
		*  @return 返回本地密码的常量引用
		*  @note 密码长度在12-32字节之间
		*  @note 密码在构造函数中自动生成
		*  @note 该方法是纯虚函数，子类必须实现
		*/
	   virtual const std::string& LocalPasswd() const = 0;
	   
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 获取远程用户名片段（Get Remote Username Fragment）
		*  
		*  该方法用于获取从SDP Offer中提取的远程用户名片段（ufrag）。
		*  
		*  @return 返回远程用户名片段的常量引用
		*  @note 远程用户名片段从SDP Offer中提取
		*  @note 该方法是纯虚函数，子类必须实现
		*/
	   virtual const std::string& RemoteUFrag() const = 0;
	   
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 构建SDP Answer（Build Answer SDP）
		*  
		*  该方法用于根据处理后的SDP Offer生成SDP Answer。Answer包含服务器选择的
		*  编解码器、ICE候选、DTLS指纹等信息。
		*  
		*  Answer生成流程：
		*  1. 创建SDP会话描述
		*  2. 添加音频/视频媒体描述，选择支持的编解码器
		*  3. 添加ICE候选（服务器地址和端口）
		*  4. 添加DTLS指纹和证书信息
		*  5. 添加用户名片段和密码
		*  6. 生成完整的SDP Answer字符串
		*  
		*  @return 返回生成的SDP Answer字符串
		*  @note 必须先调用ProcessOfferSdp()处理Offer
		*  @note Answer会根据Offer中的编解码器选择支持的格式
		*  @note 该方法是纯虚函数，子类必须实现
		*  
		*  使用示例：
		*  @code
		*  std::string answer = rtc_interface->BuildAnswerSdp();
		*  // 将answer发送给客户端
		*  @endcode
		*/
	   virtual std::string BuildAnswerSdp() = 0;

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 开始DTLS握手（May Run DTLS）
		*  
		*  该方法用于启动DTLS握手过程。根据SDP中的信息，决定作为DTLS客户端还是
		*  服务器端发起握手。DTLS握手用于交换密钥，建立SRTP加密会话。
		*  
		*  DTLS角色选择：
		*  - 如果SDP中设置了"setup:actpass"，服务器通常作为服务器端
		*  - 如果SDP中设置了"setup:active"，服务器作为客户端
		*  - 如果SDP中设置了"setup:passive"，服务器作为服务器端
		*  
		*  握手流程：
		*  1. 确定DTLS角色（客户端或服务器端）
		*  2. 加载DTLS证书和私钥
		*  3. 创建DTLS对象并连接信号
		*  4. 开始握手过程
		*  
		*  @note 该方法会在SDP处理完成后调用
		*  @note DTLS握手成功后，会触发OnDtlsConnected回调
		*  @note DTLS握手失败后，会触发OnDtlsFailed回调
		*  @note 该方法是纯虚函数，子类必须实现
		*  
		*  使用示例：
		*  @code
		*  rtc_interface->MayRunDtls();
		*  @endcode
		*/
	   virtual void MayRunDtls() = 0;
	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 发送SRTP RTP包（Send SRTP RTP）
		*  
		*  该方法用于发送SRTP加密的RTP媒体包。它会先对RTP包进行SRTP加密，
		*  然后通过UDP套接字发送给远程客户端。
		*  
		*  发送流程：
		*  1. 检查SRTP发送会话是否已建立
		*  2. 使用SRTP会话加密RTP包
		*  3. 通过UDP套接字发送加密后的包
		*  4. 更新发送统计信息
		*  
		*  @param data RTP包数据指针，包含完整的RTP头和负载
		*  @param size RTP包大小（字节）
		*  @return 如果发送成功返回true，否则返回false
		*  @note SRTP会话必须在DTLS握手成功后才能建立
		*  @note 如果SRTP会话未建立，发送会失败
		*  
		*  使用示例：
		*  @code
		*  uint8_t rtp_data[1500];
		*  size_t rtp_size = 1200;
		*  rtc_interface->SendSrtpRtp(rtp_data, rtp_size);
		*  @endcode
		*/
		bool SendSrtpRtp(uint8_t* data, size_t  size);
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 发送SRTP RTCP包（Send SRTP RTCP）
		*  
		*  该方法用于发送SRTP加密的RTCP控制包。RTCP包用于传输统计信息、
		*  反馈信息等。
		*  
		*  发送流程：
		*  1. 检查SRTP发送会话是否已建立
		*  2. 使用SRTP会话加密RTCP包
		*  3. 通过UDP套接字发送加密后的包
		*  
		*  @param data RTCP包数据指针
		*  @param size RTCP包大小（字节）
		*  @return 如果发送成功返回true，否则返回false
		*  @note RTCP包类型包括SR、RR、SDES、BYE、APP等
		*  
		*  使用示例：
		*  @code
		*  uint8_t rtcp_data[200];
		*  size_t rtcp_size = 100;
		*  rtc_interface->SendSrtpRtcp(rtcp_data, rtcp_size);
		*  @endcode
		*/
		bool SendSrtpRtcp(uint8_t* data, size_t size);

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 发送数据通道消息（Send Data Channel）
		*  
		*  该方法用于通过SCTP数据通道发送非媒体数据。数据通道可用于传输
		*  文本消息、二进制数据等。
		*  
		*  发送流程：
		*  1. 检查SCTP关联是否已建立
		*  2. 将消息封装为SCTP数据块
		*  3. 通过SCTP关联发送数据
		*  4. SCTP会通过DTLS发送数据
		*  
		*  @param streamId 流ID，用于标识数据通道
		*  @param ppid 负载协议标识符（PPID）
		*  @param msg 消息数据指针
		*  @param len 消息长度（字节）
		*  @note SCTP关联必须在DTLS握手成功后才能建立
		*  @note PPID常用值：51=WebRTC String, 53=WebRTC Binary
		*  
		*  使用示例：
		*  @code
		*  const char* message = "Hello, World!";
		*  rtc_interface->SendDatachannel(0, 51, message, strlen(message));
		*  @endcode
		*/
		void SendDatachannel(uint16_t streamId, uint32_t ppid, const char* msg, size_t len);

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 接收DTLS数据（On DTLS Receive）
		*  
		*  该方法在接收到DTLS握手数据时被调用。DTLS数据包括握手消息、
		*  应用数据等。
		*  
		*  @param buf 接收到的数据缓冲区
		*  @param size 数据大小（字节）
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 该方法在网络线程中调用
		*/
		virtual void OnDtlsRecv(const uint8_t* buf, size_t size) = 0;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 接收SRTP RTP包（On SRTP RTP）
		*  
		*  该方法在接收并解密SRTP RTP包后被调用。解密后的RTP包可以提取
		*  媒体负载并进行处理。
		*  
		*  @param data 解密后的RTP包数据指针
		*  @param size RTP包大小（字节）
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 该方法在网络线程中调用
		*/
		virtual void OnSrtpRtp(  uint8_t* data, size_t size) = 0;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 接收SRTP RTCP包（On SRTP RTCP）
		*  
		*  该方法在接收并解密SRTP RTCP包后被调用。RTCP包用于传输统计信息、
		*  反馈信息等。
		*  
		*  @param data 解密后的RTCP包数据指针
		*  @param size RTCP包大小（字节）
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 该方法在网络线程中调用
		*/
		virtual void OnSrtpRtcp(  uint8_t* data, size_t size) = 0;
	
		
	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief DTLS连接中回调（On DTLS Connecting）
		*  
		*  该方法在DTLS开始连接时被调用。此时DTLS握手正在进行中。
		*  
		*  @param dtls DTLS对象指针
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 该回调在DTLS握手开始时触发
		*/
		virtual void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief DTLS连接成功回调（On DTLS Connected）
		*  
		*  该方法在DTLS握手成功后被调用。此时可以获取到SRTP密钥材料，
		*  用于建立SRTP加密会话。
		*  
		*  处理流程：
		*  1. 获取SRTP加密套件和密钥
		*  2. 创建SRTP发送和接收会话
		*  3. 初始化SRTP会话
		*  4. 开始接收和发送SRTP包
		*  
		*  @param dtls DTLS对象指针
		*  @param srtpCryptoSuite SRTP加密套件（如AES_CM_128_HMAC_SHA1_80）
		*  @param srtpLocalKey 本地SRTP密钥指针
		*  @param srtpLocalKeyLen 本地SRTP密钥长度
		*  @param srtpRemoteKey 远程SRTP密钥指针
		*  @param srtpRemoteKeyLen 远程SRTP密钥长度
		*  @param remote_cert 远程证书指纹
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note SRTP会话在此回调中创建
		*/
		virtual void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert) = 0;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief DTLS发送数据包回调（On DTLS Send Packet）
		*  
		*  该方法在DTLS需要发送数据包时被调用。DTLS握手消息和应用数据
		*  都通过此回调发送。
		*  
		*  @param dtls DTLS对象指针
		*  @param data 待发送的数据指针
		*  @param len 数据长度（字节）
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 数据需要通过UDP套接字发送给远程客户端
		*/
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len) = 0;

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief DTLS连接关闭回调（On DTLS Closed）
		*  
		*  该方法在DTLS连接正常关闭时被调用。
		*  
		*  @param dtls DTLS对象指针
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 连接关闭后应该释放相关资源
		*/
		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief DTLS连接失败回调（On DTLS Failed）
		*  
		*  该方法在DTLS握手失败时被调用。失败原因可能是证书验证失败、
		*  超时等。
		*  
		*  @param dtls DTLS对象指针
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note 握手失败后应该关闭连接
		*/
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls) = 0;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief DTLS应用数据接收回调（On DTLS Application Data Received）
		*  
		*  该方法在接收到DTLS应用数据时被调用。应用数据通常是SCTP数据包。
		*  
		*  @param dtls DTLS对象指针
		*  @param data 应用数据指针
		*  @param len 数据长度（字节）
		*  @note 该方法是纯虚函数，子类必须实现
		*  @note SCTP数据包会通过此回调接收
		*/
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len) = 0;
	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 发送TWCC反馈（On Send TWCC）
		*  
		*  该方法用于发送TWCC（Transport-wide Congestion Control）反馈信息。
		*  TWCC用于带宽估计和拥塞控制。
		*  
		*  处理流程：
		*  1. 构建RTCP TWCC反馈包
		*  2. 包含接收到的RTP包的序列号和到达时间
		*  3. 通过SRTP发送RTCP包
		*  
		*  @param ssrc 同步源标识符（SSRC）
		*  @param twcc_fci TWCC反馈控制信息（FCI）
		*  @note TWCC用于WebRTC的带宽估计算法
		*  @note 反馈信息帮助发送端调整发送码率
		*  
		*  使用示例：
		*  @code
		*  uint32_t ssrc = 12345678;
		*  std::string fci = "..."; // TWCC FCI数据
		*  rtc_interface->onSendTwcc(ssrc, fci);
		*  @endcode
		*/
		void onSendTwcc(uint32_t ssrc, const std::string& twcc_fci);

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 创建数据通道（Create Data Channel）
		*  
		*  该方法用于创建SCTP数据通道。数据通道用于传输非媒体数据，
		*  如文本消息、二进制数据等。
		*  
		*  创建流程：
		*  1. 检查DTLS是否已连接
		*  2. 创建SCTP关联
		*  3. 配置SCTP参数
		*  4. 开始SCTP握手
		*  
		*  @note 数据通道必须在DTLS连接成功后才能创建
		*  @note SCTP关联通过DTLS传输
		*  
		*  使用示例：
		*  @code
		*  rtc_interface->CreateDataChannel();
		*  @endcode
		*/
		void CreateDataChannel();

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 添加视频包（Add Video Packet）
		*  
		*  该方法用于添加视频RTP包到发送缓存。这些包会被缓存起来，
		*  以便在收到NACK请求时进行重传。
		*  
		*  处理流程：
		*  1. 将RTP包添加到缓存映射表
		*  2. 使用序列号作为键
		*  3. 定期清理过期的包
		*  
		*  @param rtp_packet RTP包的共享指针
		*  @note 缓存的包用于NACK重传
		*  @note 缓存大小有限制，会自动清理旧包
		*  
		*  使用示例：
		*  @code
		*  auto rtp_packet = std::make_shared<libmedia_transfer_protocol::RtpPacketToSend>();
		*  rtc_interface->AddVideoPacket(rtp_packet);
		*  @endcode
		*/
		void AddVideoPacket(std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend> rtp_packet);

		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 请求NACK重传（Request NACK）
		*  
		*  该方法用于处理接收到的NACK请求。当接收端检测到丢包时，
		*  会发送NACK请求，要求重传丢失的包。
		*  
		*  处理流程：
		*  1. 解析NACK请求中的丢包序列号
		*  2. 从缓存中查找对应的RTP包
		*  3. 重新发送丢失的包
		*  
		*  @param nack NACK请求对象，包含丢包序列号列表
		*  @note 只有缓存中的包才能重传
		*  @note NACK机制用于提高视频质量
		*  
		*  使用示例：
		*  @code
		*  libmedia_transfer_protocol::rtcp::Nack nack;
		*  // ... 填充NACK信息
		*  rtc_interface->RequestNack(nack);
		*  @endcode
		*/
		void RequestNack(const libmedia_transfer_protocol::rtcp::Nack& nack);
	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief SCTP关联连接中回调（On SCTP Association Connecting）
		*  
		*  该方法在SCTP关联开始连接时被调用。此时SCTP握手正在进行中。
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @note 该方法覆盖父类的虚函数
		*  @note SCTP关联通过DTLS传输
		*/
		virtual void OnSctpAssociationConnecting(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief SCTP关联连接成功回调（On SCTP Association Connected）
		*  
		*  该方法在SCTP关联连接成功后被调用。此时可以开始通过数据通道
		*  发送和接收数据。
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @note 该方法覆盖父类的虚函数
		*  @note 连接成功后可以创建数据通道
		*/
		virtual void OnSctpAssociationConnected(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief SCTP关联连接失败回调（On SCTP Association Failed）
		*  
		*  该方法在SCTP关联连接失败时被调用。失败原因可能是超时、
		*  协议错误等。
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @note 该方法覆盖父类的虚函数
		*  @note 连接失败后应该关闭关联
		*/
		virtual void OnSctpAssociationFailed(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief SCTP关联关闭回调（On SCTP Association Closed）
		*  
		*  该方法在SCTP关联正常关闭时被调用。
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @note 该方法覆盖父类的虚函数
		*  @note 关闭后应该释放相关资源
		*/
		virtual void OnSctpAssociationClosed(libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation) override;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief SCTP关联发送数据回调（On SCTP Association Send Data）
		*  
		*  该方法在SCTP关联需要发送数据时被调用。SCTP数据会通过DTLS发送。
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @param data 待发送的数据指针
		*  @param len 数据长度（字节）
		*  @note 该方法覆盖父类的虚函数
		*  @note SCTP数据通过DTLS应用数据发送
		*/
		virtual void OnSctpAssociationSendData(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation, 
			const uint8_t* data, size_t len)  override;
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief SCTP关联接收消息回调（On SCTP Association Message Received）
		*  
		*  该方法在接收到SCTP数据通道消息时被调用。消息可以是文本或二进制数据。
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @param streamId 流ID，用于标识数据通道
		*  @param ppid 负载协议标识符（PPID）
		*  @param msg 消息数据指针
		*  @param len 消息长度（字节）
		*  @note 该方法覆盖父类的虚函数
		*  @note PPID常用值：51=WebRTC String, 53=WebRTC Binary
		*/
		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;

	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 获取RTC远程地址（Get RTC Remote Address）
		*  
		*  该方法用于获取远程客户端的网络地址（IP和端口）。
		*  
		*  @return 返回远程地址的常量引用
		*  @note 远程地址在接收到第一个数据包时设置
		*  @note 用于发送RTP/RTCP包到远程客户端
		*  
		*  使用示例：
		*  @code
		*  const rtc::SocketAddress& addr = rtc_interface->RtcRemoteAddress();
		*  std::cout << "Remote IP: " << addr.ipaddr().ToString() << std::endl;
		*  @endcode
		*/
		virtual  const rtc::SocketAddress& RtcRemoteAddress() const
		{
			return rtc_remote_address_;
		}
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 设置RTC远程地址（Set RTC Remote Address）
		*  
		*  该方法用于设置远程客户端的网络地址（IP和端口）。
		*  
		*  @param addr 远程地址对象
		*  @note 远程地址用于发送数据包
		*  @note 通常在接收到第一个数据包时自动设置
		*  
		*  使用示例：
		*  @code
		*  rtc::SocketAddress addr("192.168.1.100", 12345);
		*  rtc_interface->SetRtcRemoteAddress(addr);
		*  @endcode
		*/
		virtual void  SetRtcRemoteAddress(const rtc::SocketAddress& addr);

	public:
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 生成用户名片段（Get Username Fragment）
		*  
		*  该静态方法用于生成随机的ICE用户名片段（ufrag）。用户名片段用于
		*  ICE连接建立。
		*  
		*  生成规则：
		*  1. 使用随机数生成器
		*  2. 生成指定长度的字符串
		*  3. 字符集包括字母和数字
		*  
		*  @param size 用户名片段长度
		*  @return 返回生成的用户名片段字符串
		*  @note 用户名片段长度通常为4-8字节
		*  
		*  使用示例：
		*  @code
		*  std::string ufrag = RtcInterface::GetUFrag(4);
		*  std::cout << "UFrag: " << ufrag << std::endl;
		*  @endcode
		*/
		static std::string GetUFrag(int size);
		
		/**
		*  @author chensong
		*  @date 2025-11-03
		*  @brief 生成同步源标识符（Get SSRC）
		*  
		*  该静态方法用于生成随机的SSRC（Synchronization Source）。
		*  SSRC用于唯一标识RTP流。
		*  
		*  生成规则：
		*  1. 使用随机数生成器
		*  2. 生成32位无符号整数
		*  3. 确保SSRC的唯一性
		*  
		*  @param size 未使用的参数（保留）
		*  @return 返回生成的SSRC值
		*  @note SSRC必须在所有RTP流中唯一
		*  
		*  使用示例：
		*  @code
		*  uint32_t ssrc = RtcInterface::GetSsrc(0);
		*  std::cout << "SSRC: " << ssrc << std::endl;
		*  @endcode
		*/
		static uint32_t GetSsrc(int size);
	protected:
		// ICE相关参数
		std::string local_ufrag_;              ///< 本地ICE用户名片段，用于ICE连接建立
		std::string local_passwd_;             ///< 本地ICE密码，长度范围[12, 32]字节
		
		// SDP相关
		libmedia_transfer_protocol::librtc::RtcSdp sdp_;  ///< SDP对象，用于存储和处理SDP信息

		// DTLS相关
		libmedia_transfer_protocol::libssl::Dtls   dtls_;  ///< DTLS对象，用于密钥交换和加密
		bool dtls_done_{ false };              ///< DTLS握手完成标志

		// 网络地址
		rtc::SocketAddress             rtc_remote_address_;  ///< 远程客户端地址（IP和端口）
		
		// SRTP会话
		libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_send_session_;  ///< SRTP发送会话，用于加密发送的RTP/RTCP包
		libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_recv_session_;  ///< SRTP接收会话，用于解密接收的RTP/RTCP包

		// RTP序列号
		uint32_t      audio_seq_ = 100;        ///< 音频RTP序列号，从100开始递增
		uint32_t      video_seq_ = 100;        ///< 视频RTP序列号，从100开始递增
		uint32_t      video_rtx_seq_ = 100;    ///< 视频RTX序列号，用于重传包

		// RTP头部和扩展
		libmedia_transfer_protocol::RTPHeader  rtp_header_;  ///< RTP头部对象，用于构建RTP包
		libmedia_transfer_protocol::RtpHeaderExtensionMap    extension_manager_;  ///< RTP扩展头管理器
		
		// TWCC上下文
		libmedia_transfer_protocol::librtcp::TwccContext     twcc_context_;  ///< TWCC上下文，用于带宽估计

		// SCTP关联
		std::shared_ptr< libmedia_transfer_protocol::librtc::SctpAssociationImp> sctp_;  ///< SCTP关联对象，用于数据通道

		// RTP包缓存
		//std::unordered_map<uint32_t, std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend>>   rtp_video_packets_;  ///< 视频RTP包缓存，用于NACK重传
		std::map<uint32_t, std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend>>   rtp_video_packets_;  ///< 视频RTP包缓存，用于NACK重传
	};
 
}
#endif // _C_GB_MEDIA_SERVER_RTC_INTERFACE_H_