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
				   date:  2025-10-13

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


#ifndef _C_GB_MEDIA_SERVER_RTC_PLAY_USER_H_
#define _C_GB_MEDIA_SERVER_RTC_PLAY_USER_H_

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
#include "consumer/rtc_consumer.h"
#include "share/rtc_interface.h"
namespace gb_media_server {
	/**
	*  @author chensong
	*  @date 2025-10-13
	*  @brief RTC消费者类（RTC Consumer）
	*  
	*  RtcConsumer是GBMediaServer流媒体服务器中用于WebRTC协议播放的消费者类。
	*  它继承自RtcInterface和Consumer基类，负责将服务器中的媒体流封装为RTP/RTCP包
	*  并通过WebRTC协议发送给客户端。
	*  
	*  WebRTC协议说明：
	*  - WebRTC（Web Real-Time Communication）是一种实时通信技术，支持低延迟的
	*    音视频传输和数据通道
	*  - WebRTC使用SDP（Session Description Protocol）进行信令交换
	*  - WebRTC使用DTLS（Datagram Transport Layer Security）进行密钥交换
	*  - WebRTC使用SRTP（Secure Real-time Transport Protocol）进行媒体加密传输
	*  - WebRTC使用SCTP（Stream Control Transmission Protocol）进行数据通道传输
	*  
	*  RtcConsumer功能：
	*  1. 接收来自Stream的视频帧和音频帧
	*  2. 将视频帧和音频帧封装为RTP包并加密发送
	*  3. 处理RTCP控制包（接收报告、发送报告、反馈等）
	*  4. 支持DTLS握手和SRTP加密
	*  5. 支持SCTP数据通道
	*  6. 支持关键帧请求（PLI/FIR）
	*  
	*  工作流程：
	*  1. 客户端通过HTTP API发送SDP Offer
	*  2. RtcConsumer处理SDP Offer并生成SDP Answer
	*  3. 开始DTLS握手，交换证书和密钥
	*  4. DTLS握手成功后，建立SRTP会话
	*  5. 建立SCTP关联，支持数据通道
	*  6. 开始发送RTP媒体包和RTCP控制包
	*  
	*  RTP/RTCP说明：
	*  - RTP（Real-time Transport Protocol）用于传输音视频数据
	*  - RTCP（RTP Control Protocol）用于传输控制信息（丢包统计、带宽估计等）
	*  - RTP包包含序列号、时间戳、SSRC等头部信息
	*  - RTCP包用于反馈接收质量、请求关键帧等
	*  
	*  @note RtcConsumer使用RtcInterface基类提供的DTLS、SRTP和SCTP功能
	*  @note 视频帧会被封装为H264格式的RTP包
	*  @note 音频帧会被封装为OPUS或PCMU格式的RTP包
	*  @note 支持RTX重传机制，提高传输可靠性
	*  
	*  使用示例：
	*  @code
	*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
	*  session->AddConsumer(consumer);
	*  @endcode
	*/
	class RtcConsumer : public  RtcInterface,  public  Consumer // ,
//#if TEST_RTC_PLAY
//		public  libmedia_codec::EncodeImageObser, public libmedia_codec::EncodeAudioObser,
//#endif // 
	//	public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-13
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于创建RtcConsumer实例。它会初始化流对象、会话对象、
		*  SDP对象、DTLS对象等，并设置音频和视频的SSRC。
		*  
		*  初始化流程：
		*  1. 调用基类RtcInterface和Consumer的构造函数
		*  2. 生成音频和视频的SSRC（Synchronization Source Identifier）
		*  3. 初始化SDP对象，设置音频/视频SSRC和RTX SSRC
		*  4. 初始化DTLS对象，连接DTLS信号
		*  5. 设置SDP的本地指纹、服务器地址和端口
		*  6. 设置流名称
		*  
		*  SSRC说明：
		*  - SSRC用于标识RTP流的源，每个媒体流有唯一的SSRC
		*  - 音频SSRC和视频SSRC通常是连续的
		*  - RTX SSRC用于重传机制，通常是视频SSRC+1
		*  
		*  @param stream 流对象引用，用于获取媒体流，不能为空
		*  @param s 会话对象引用，用于管理会话状态，不能为空
		*  @note 流对象和会话对象通过引用传递，确保生命周期管理正确
		*  @note DTLS信号连接会在构造函数中完成
		*  
		*  使用示例：
		*  @code
		*  auto consumer = std::make_shared<RtcConsumer>(stream, session);
		*  @endcode
		*/
		explicit RtcConsumer(   std::shared_ptr<Stream> &stream, const  std::shared_ptr<Session> &s);

		/**
		*  @author chensong
		*  @date 2025-10-13
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtcConsumer实例。它会释放所有相关资源，
		*  包括DTLS对象、SRTP会话、SCTP关联等。
		*  
		*  清理流程：
		*  1. 断开DTLS连接（如果已连接）
		*  2. 关闭SRTP会话
		*  3. 关闭SCTP关联
		*  4. 调用基类的析构函数进行基类资源清理
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*  @note 流对象和会话对象由外部管理，不需要在此释放
		*/
		virtual ~RtcConsumer();


		

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 请求关键帧（Request Key Frame）
		*  
		*  该方法用于向生产者请求发送关键帧（IDR帧）。当客户端解码器需要重新同步
		*  或网络恢复后需要快速恢复播放时，会调用该方法请求关键帧。
		*  
		*  关键帧请求流程：
		*  1. 客户端通过RTCP PLI（Picture Loss Indication）或FIR（Full Intra Request）
		*     包请求关键帧
		*  2. RtcConsumer接收到请求后，调用该方法
		*  3. 该方法通知Session，Session再通知Producer生成关键帧
		*  4. Producer生成关键帧后，通过Stream分发给所有Consumer
		*  
		*  关键帧说明：
		*  - 关键帧（IDR帧）是可以独立解码的帧，不依赖其他帧
		*  - 关键帧包含完整的图像信息，文件大小通常较大
		*  - 关键帧前会附加SPS和PPS参数集
		*  
		*  @note 该方法通常由RTCP反馈机制触发
		*  @note 频繁请求关键帧会增加带宽消耗，应控制请求频率
		*/
		void RequestKeyFrame();
	
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理Offer SDP（Process Offer SDP）
		*  
		*  该方法用于处理客户端发送的SDP Offer，解析媒体能力、ICE候选、DTLS指纹等信息，
		*  并生成本地的SDP Answer。
		*  
		*  处理流程：
		*  1. 解析客户端的SDP Offer，提取音视频编解码器、ICE候选、DTLS指纹等信息
		*  2. 验证SDP格式是否正确，检查必要的字段是否存在
		*  3. 提取远程的ICE用户名和密码
		*  4. 提取远程的DTLS指纹，用于DTLS握手验证
		*  5. 设置本地的媒体能力（支持的编解码器、扩展等）
		*  6. 生成本地的SDP Answer，包含本地的ICE候选、DTLS指纹等
		*  
		*  SDP说明：
		*  - SDP（Session Description Protocol）用于描述多媒体会话的参数
		*  - SDP Offer由客户端发送，描述客户端的媒体能力
		*  - SDP Answer由服务器发送，描述服务器的媒体能力
		*  - SDP包含媒体类型、编解码器、传输协议、ICE候选等信息
		*  
		*  @param rtc_sdp_type SDP类型（Offer或Answer）
		*  @param sdp SDP字符串，包含完整的SDP描述
		*  @return 处理成功返回true，失败返回false
		*  @note 该方法必须在DTLS握手前调用
		*  @note SDP格式必须符合RFC 4566标准
		*/
		virtual  bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string &sdp);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取本地ICE用户名片段（Local UFrag）
		*  
		*  该方法用于获取本地的ICE用户名片段（Username Fragment），用于ICE连接检查。
		*  
		*  ICE说明：
		*  - ICE（Interactive Connectivity Establishment）是一种NAT穿透技术
		*  - UFrag是ICE用户名的一部分，用于标识ICE会话
		*  - UFrag通常是随机生成的字符串，长度为4-256个字符
		*  
		*  @return 本地ICE用户名片段
		*  @note UFrag在构造函数中生成，不会改变
		*/
		virtual const std::string &LocalUFrag() const;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取本地ICE密码（Local Password）
		*  
		*  该方法用于获取本地的ICE密码，用于ICE连接检查的认证。
		*  
		*  ICE密码说明：
		*  - ICE密码用于验证ICE连接请求的合法性
		*  - 密码长度通常为22-256个字符
		*  - 密码在SDP交换时传递给对端
		*  
		*  @return 本地ICE密码
		*  @note 密码在构造函数中生成，不会改变
		*/
		virtual const std::string &LocalPasswd() const;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取远程ICE用户名片段（Remote UFrag）
		*  
		*  该方法用于获取远程的ICE用户名片段，从客户端的SDP Offer中提取。
		*  
		*  @return 远程ICE用户名片段
		*  @note 该值在ProcessOfferSdp方法中从SDP中提取
		*/
		virtual const std::string &RemoteUFrag() const;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 构建Answer SDP（Build Answer SDP）
		*  
		*  该方法用于生成本地的SDP Answer，包含本地的媒体能力、ICE候选、DTLS指纹等信息。
		*  
		*  生成流程：
		*  1. 设置会话级别的信息（版本、源、时间等）
		*  2. 添加音频媒体描述（编解码器、SSRC、扩展等）
		*  3. 添加视频媒体描述（编解码器、SSRC、扩展、RTX等）
		*  4. 添加ICE候选（本地IP地址和端口）
		*  5. 添加DTLS指纹（用于DTLS握手验证）
		*  6. 添加SCTP数据通道描述（如果支持）
		*  7. 序列化为SDP字符串
		*  
		*  @return SDP Answer字符串
		*  @note 该方法必须在ProcessOfferSdp之后调用
		*  @note 生成的SDP Answer需要发送给客户端
		*/
		virtual std::string BuildAnswerSdp();

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 启动DTLS握手（May Run DTLS）
		*  
		*  该方法用于启动DTLS握手，根据SDP中的setup属性决定是作为客户端还是服务端。
		*  
		*  DTLS握手流程：
		*  1. 检查SDP交换是否完成
		*  2. 根据SDP中的setup属性决定角色：
		*     - setup=active：本地作为DTLS客户端，主动发起握手
		*     - setup=passive：本地作为DTLS服务端，等待对端发起握手
		*     - setup=actpass：本地可以作为客户端或服务端，由对端决定
		*  3. 启动DTLS握手，交换证书和密钥
		*  4. 握手成功后，建立SRTP会话
		*  
		*  DTLS说明：
		*  - DTLS（Datagram Transport Layer Security）是TLS的UDP版本
		*  - DTLS用于在不可靠的传输层上提供安全通信
		*  - DTLS握手会交换证书、验证身份、协商加密算法
		*  
		*  @note 该方法通常在ICE连接建立后调用
		*  @note DTLS握手失败会导致媒体传输失败
		*/
		virtual void MayRunDtls();
	public:

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理接收到的DTLS数据（On DTLS Receive）
		*  
		*  该方法用于处理从网络接收到的DTLS数据包，将其传递给DTLS对象进行处理。
		*  
		*  处理流程：
		*  1. 接收从UDP socket读取的DTLS数据包
		*  2. 将数据包传递给DTLS对象进行解密和处理
		*  3. DTLS对象会触发相应的回调（OnDtlsConnected、OnDtlsApplicationDataReceived等）
		*  
		*  @param buf DTLS数据包缓冲区
		*  @param size 数据包大小
		*  @note 该方法由RtcService在接收到DTLS包时调用
		*  @note DTLS握手期间会频繁调用该方法
		*/
		virtual void OnDtlsRecv(const uint8_t *buf, size_t size);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理SRTP RTP包（On SRTP RTP）
		*  
		*  该方法用于处理从客户端接收到的SRTP RTP包，解密后提取媒体数据。
		*  
		*  处理流程：
		*  1. 接收加密的SRTP RTP包
		*  2. 使用SRTP会话解密RTP包
		*  3. 解析RTP头部，提取序列号、时间戳、SSRC等信息
		*  4. 提取RTP Payload，包含音频或视频编码数据
		*  5. 根据Payload Type判断是音频还是视频
		*  6. 将媒体数据传递给相应的处理模块
		*  
		*  RTP包格式：
		*  - RTP头部（12字节）：版本、填充、扩展、CSRC计数、标记、Payload Type、序列号、时间戳、SSRC
		*  - RTP扩展（可选）：扩展头部ID、扩展数据
		*  - RTP Payload：音频或视频编码数据
		*  
		*  @param data RTP数据包缓冲区（已解密）
		*  @param size 数据包大小
		*  @note 该方法由RtcService在接收到RTP包时调用
		*  @note RTP包必须先通过SRTP解密才能处理
		*/
		virtual void OnSrtpRtp(  uint8_t * data, size_t size);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理SRTP RTCP包（On SRTP RTCP）
		*  
		*  该方法用于处理从客户端接收到的SRTP RTCP包，解密后提取控制信息。
		*  
		*  处理流程：
		*  1. 接收加密的SRTP RTCP包
		*  2. 使用SRTP会话解密RTCP包
		*  3. 解析RTCP包类型（SR、RR、SDES、BYE、APP、RTPFB、PSFB等）
		*  4. 根据RTCP包类型进行相应处理：
		*     - SR（Sender Report）：发送者报告，包含发送统计信息
		*     - RR（Receiver Report）：接收者报告，包含接收统计信息
		*     - RTPFB（RTP Feedback）：RTP反馈，包含NACK重传请求
		*     - PSFB（Payload-Specific Feedback）：负载特定反馈，包含PLI/FIR关键帧请求
		*  5. 更新带宽估计、丢包统计等信息
		*  6. 根据反馈调整发送策略（码率、关键帧等）
		*  
		*  RTCP包类型：
		*  - SR（200）：发送者报告
		*  - RR（201）：接收者报告
		*  - SDES（202）：源描述
		*  - BYE（203）：结束会话
		*  - APP（204）：应用定义
		*  - RTPFB（205）：RTP反馈（NACK等）
		*  - PSFB（206）：负载特定反馈（PLI、FIR等）
		*  
		*  @param data RTCP数据包缓冲区（已解密）
		*  @param size 数据包大小
		*  @note 该方法由RtcService在接收到RTCP包时调用
		*  @note RTCP包用于控制和反馈，不包含媒体数据
		*/
		virtual void OnSrtpRtcp(  uint8_t * data, size_t size);
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理视频帧（On Video Frame）
		*  
		*  该方法用于处理从Stream接收到的视频帧，将其封装为RTP包并通过SRTP发送。
		*  
		*  处理流程：
		*  1. 接收H264编码的视频帧
		*  2. 将视频帧分片为多个RTP包（如果帧太大）
		*  3. 设置RTP头部信息（Payload Type、时间戳、序列号、SSRC等）
		*  4. 添加RTP扩展（如传输序列号、绝对发送时间等）
		*  5. 使用SRTP会话加密RTP包
		*  6. 通过UDP socket发送加密后的RTP包
		*  
		*  RTP打包策略：
		*  - 单NALU模式：如果NALU小于MTU，直接封装为一个RTP包
		*  - FU-A分片模式：如果NALU大于MTU，分片为多个RTP包
		*  - STAP-A聚合模式：如果多个小NALU可以合并，聚合为一个RTP包
		*  
		*  @param frame 编码后的视频帧，包含H264编码数据和元信息
		*  @note 该方法会在Stream分发视频帧时被调用
		*  @note 视频帧会被分片为多个RTP包，每个包不超过MTU大小
		*  @note RTP包会通过SRTP加密后发送
		*/
		virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理音频帧（On Audio Frame）
		*  
		*  该方法用于处理从Stream接收到的音频帧，将其封装为RTP包并通过SRTP发送。
		*  
		*  处理流程：
		*  1. 接收音频编码数据和PTS时间戳
		*  2. 创建RTP包，设置Payload Type、时间戳、序列号、SSRC等
		*  3. 将音频编码数据复制到RTP Payload
		*  4. 添加RTP扩展（如传输序列号等）
		*  5. 使用SRTP会话加密RTP包
		*  6. 通过UDP socket发送加密后的RTP包
		*  
		*  音频RTP打包：
		*  - 音频帧通常较小，一个RTP包可以包含一个或多个音频帧
		*  - 音频时间戳需要转换为RTP时间戳（通常乘以90）
		*  - 音频编码格式可能为OPUS、PCMU、PCMA等
		*  
		*  @param frame 音频编码数据缓冲区
		*  @param pts 呈现时间戳（Presentation Time Stamp）
		*  @note 该方法会在Stream分发音频帧时被调用
		*  @note 音频帧通常不需要分片，一个RTP包即可
		*/
		virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);

	public:

	
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS连接中回调（On DTLS Connecting）
		*  
		*  该方法是DTLS握手过程中的回调，当DTLS开始连接时触发。
		*  
		*  @param dtls DTLS对象指针
		*  @note 该方法由DTLS对象在握手开始时调用
		*  @note 可以在此记录日志或更新连接状态
		*/
		virtual void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS连接成功回调（On DTLS Connected）
		*  
		*  该方法是DTLS握手成功后的回调，用于建立SRTP会话和SCTP关联。
		*  
		*  处理流程：
		*  1. DTLS握手成功，获取加密套件和密钥材料
		*  2. 使用密钥材料创建SRTP发送会话和接收会话
		*  3. 建立SCTP关联，用于数据通道传输
		*  4. 设置dtls_done_标志为true，表示可以开始发送媒体数据
		*  5. 验证远程证书指纹，确保连接安全
		*  
		*  SRTP密钥导出：
		*  - DTLS握手成功后，会导出SRTP密钥材料
		*  - 密钥材料包括本地密钥和远程密钥
		*  - 本地密钥用于加密发送的RTP/RTCP包
		*  - 远程密钥用于解密接收的RTP/RTCP包
		*  
		*  @param dtls DTLS对象指针
		*  @param srtpCryptoSuite SRTP加密套件（如AES_CM_128_HMAC_SHA1_80）
		*  @param srtpLocalKey 本地SRTP密钥
		*  @param srtpLocalKeyLen 本地密钥长度
		*  @param srtpRemoteKey 远程SRTP密钥
		*  @param srtpRemoteKeyLen 远程密钥长度
		*  @param remote_cert 远程证书指纹
		*  @note 该方法由DTLS对象在握手成功时调用
		*  @note SRTP会话必须在此建立，否则无法发送媒体数据
		*/
		virtual void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS发送数据包回调（On DTLS Send Packet）
		*  
		*  该方法是DTLS对象请求发送数据包时的回调，用于将DTLS握手包发送到网络。
		*  
		*  处理流程：
		*  1. DTLS对象生成握手包（ClientHello、ServerHello、Certificate等）
		*  2. 调用该回调，请求发送握手包
		*  3. 将握手包通过UDP socket发送到远程地址
		*  
		*  @param dtls DTLS对象指针
		*  @param data 数据包缓冲区
		*  @param len 数据包长度
		*  @note 该方法由DTLS对象在需要发送数据时调用
		*  @note DTLS握手期间会多次调用该方法
		*/
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t *data, size_t len);
		 
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS连接关闭回调（On DTLS Closed）
		*  
		*  该方法是DTLS连接正常关闭时的回调，用于清理资源。
		*  
		*  处理流程：
		*  1. DTLS连接关闭
		*  2. 关闭SRTP会话
		*  3. 关闭SCTP关联
		*  4. 清理相关资源
		*  
		*  @param dtls DTLS对象指针
		*  @note 该方法由DTLS对象在连接关闭时调用
		*  @note 正常关闭不会触发错误，只是清理资源
		*/
		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls *dtls);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS连接失败回调（On DTLS Failed）
		*  
		*  该方法是DTLS握手失败时的回调，用于记录错误并清理资源。
		*  
		*  失败原因：
		*  - 证书验证失败
		*  - 握手超时
		*  - 加密套件不匹配
		*  - 网络错误
		*  
		*  @param dtls DTLS对象指针
		*  @note 该方法由DTLS对象在握手失败时调用
		*  @note 握手失败会导致媒体传输失败，需要重新建立连接
		*/
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls *dtls);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS应用数据接收回调（On DTLS Application Data Received）
		*  
		*  该方法是DTLS接收到应用数据时的回调，用于处理SCTP数据通道的数据。
		*  
		*  处理流程：
		*  1. DTLS接收到应用数据（SCTP包）
		*  2. 将数据传递给SCTP关联进行处理
		*  3. SCTP关联解析数据，提取数据通道消息
		*  4. 触发OnDataChannel回调，处理数据通道消息
		*  
		*  SCTP over DTLS：
		*  - WebRTC使用SCTP over DTLS传输数据通道消息
		*  - DTLS提供加密和认证
		*  - SCTP提供可靠或不可靠的消息传输
		*  
		*  @param dtls DTLS对象指针
		*  @param data 应用数据缓冲区
		*  @param len 数据长度
		*  @note 该方法由DTLS对象在接收到应用数据时调用
		*  @note 应用数据通常是SCTP包
		*/
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls *dtls, const uint8_t* data, size_t len);
		 
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 数据通道消息回调（On Data Channel）
		*  
		*  该方法用于处理从客户端接收到的数据通道消息。
		*  
		*  数据通道说明：
		*  - WebRTC数据通道基于SCTP协议
		*  - 数据通道可以传输任意二进制数据或文本数据
		*  - 数据通道支持可靠传输和不可靠传输
		*  - 数据通道可以用于传输控制消息、文件、聊天消息等
		*  
		*  @param params SCTP流参数，包含流ID、可靠性等信息
		*  @param ppid Payload Protocol Identifier，标识数据类型
		*  @param msg 消息数据缓冲区
		*  @param len 消息长度
		*  @note 该方法由SCTP关联在接收到数据通道消息时调用
		*  @note PPID用于标识数据类型（如字符串、二进制、空消息等）
		*/
		virtual void OnDataChannel(const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, uint32_t ppid, const uint8_t* msg, size_t len);
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief SCTP关联消息接收回调（On SCTP Association Message Received）
		*  
		*  该方法是SCTP关联接收到消息时的回调，用于处理数据通道消息。
		*  
		*  处理流程：
		*  1. SCTP关联接收到消息
		*  2. 解析SCTP流ID和PPID
		*  3. 根据流ID查找对应的数据通道
		*  4. 调用OnDataChannel方法处理消息
		*  
		*  SCTP流说明：
		*  - SCTP支持多个流，每个流有独立的序列号
		*  - 每个数据通道对应一个SCTP流
		*  - 流ID用于标识数据通道
		*  
		*  @param sctpAssociation SCTP关联对象指针
		*  @param streamId SCTP流ID，标识数据通道
		*  @param ppid Payload Protocol Identifier，标识数据类型
		*  @param msg 消息数据缓冲区
		*  @param len 消息长度
		*  @note 该方法由SCTP关联在接收到消息时调用
		*  @note 该方法会调用OnDataChannel进行实际处理
		*/
		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取共享资源类型（Share Resource Type）
		*  
		*  该方法用于标识该对象的类型为RTC消费者。
		*  
		*  @return 返回kConsumerTypeRTC，表示这是一个RTC消费者
		*  @note 该方法用于类型识别和资源管理
		*/
		virtual ShareResourceType ShareResouceType() const   { return kConsumerTypeRTC; }
		 
	private:
		//static std::string GetUFrag(int size);
		//static uint32_t GetSsrc(int size);
	private:
		// std::string local_ufrag_;
		// std::string local_passwd_;  //[12, 32]
		// libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		// //Dtls dtls_;
		// 
		// //libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		// libmedia_transfer_protocol::libssl::Dtls   dtls_;
		// 
		// bool dtls_done_{ false };
		// 
		// //rtc::SocketAddress             remote_address_;
		// libmedia_transfer_protocol::libsrtp::SrtpSession*   srtp_send_session_;
		// libmedia_transfer_protocol::libsrtp::SrtpSession *  srtp_recv_session_;
		// 
		// 
		// 
		// 
		// uint32_t      audio_seq_ = 100;
		// uint32_t      video_seq_ = 100;
		//  libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;

		
		

	};
}

#endif // _C_WEBRTC_PLAY_H_