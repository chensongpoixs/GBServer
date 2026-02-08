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


#ifndef _C_GB_MEDIA_SERVER_RTSP_Producer_H_
#define _C_GB_MEDIA_SERVER_RTSP_Producer_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
 
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_received.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "libmedia_transfer_protocol/rtp_video_frame_assembler.h"
#include "producer/producer.h"
#include "share/rtc_interface.h"
#include "api/array_view.h"

namespace gb_media_server {

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTSP生产者类（RTSP Producer）
	*  
	*  RtspProducer是GBMediaServer流媒体服务器中用于处理RTSP推流的生产者类。
	*  它负责接收来自RTSP客户端的媒体数据，解析RTSP协议和RTP/RTCP数据包，
	*  提取音视频数据，并将这些数据推送到Stream中，供消费者使用。
	*  
	*  RTSP协议说明：
	*  - RTSP（Real Time Streaming Protocol）是一种网络控制协议
	*  - RTSP使用TCP传输控制信息，使用RTP/RTCP传输媒体数据
	*  - RTSP支持两种传输模式：UDP模式和TCP Interleaved模式
	*  - TCP Interleaved模式：RTP/RTCP数据通过TCP连接传输，使用特殊的帧格式
	*  
	*  TCP Interleaved帧格式：
	*  - Magic Byte: '$' (0x24)
	*  - Channel ID: 0-255（0/1用于RTP/RTCP，2/3用于音频RTP/RTCP等）
	*  - Length: 2字节，大端序，表示payload长度
	*  - Payload: RTP或RTCP数据包
	*  
	*  工作流程：
	*  1. 接收来自RTSP客户端的TCP数据包
	*  2. 解析RTSP Interleaved帧头（Magic、Channel、Length）
	*  3. 根据Channel ID判断是RTP还是RTCP
	*  4. 解析RTP数据包，提取媒体数据
	*  5. 解析RTCP数据包，处理控制信息
	*  6. 将媒体数据推送到Stream中
	*  
	*  @note RtspProducer继承自Producer和sigslot::has_slots<>
	*  @note 支持RTSP TCP Interleaved模式
	*  @note 支持RTP/RTCP协议解析
	*  @note 支持H.264视频编码
	*  
	*  使用示例：
	*  @code
	*  // 创建RTSP生产者
	*  auto producer = std::make_shared<RtspProducer>(stream, session);
	*  
	*  // 接收RTSP数据
	*  producer->OnRecv(buffer);
	*  @endcode
	*/
	class RtspProducer  :   public Producer, public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化RtspProducer实例。它会创建RTP解析器，
		*  初始化接收缓冲区，并准备接收RTSP数据。
		*  
		*  初始化流程：
		*  1. 调用Producer基类构造函数，关联Stream和Session
		*  2. 创建RTP视频帧组装器（用于组装分片的RTP包）
		*  3. 分配接收缓冲区（8MB大小）
		*  4. 初始化SSRC（音频和视频）
		*  
		*  @param stream 指向Stream的共享指针，用于推送媒体数据
		*  @param s 指向Session的共享指针，用于管理会话状态
		*  @note 构造函数会自动创建RTP解析器和接收缓冲区
		*  @note 接收缓冲区大小为8MB，用于缓存不完整的数据包
		*/
		explicit RtspProducer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtspProducer实例。它会释放所有相关资源，
		*  包括RTP解析器、接收缓冲区等。
		*  
		*  清理流程：
		*  1. 记录日志，标记析构开始
		*  2. 释放接收缓冲区
		*  3. 释放RTP解析器
		*  4. 清理所有资源
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual ~RtspProducer();
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收数据（On Receive）
		*  
		*  该方法用于接收来自RTSP客户端的TCP数据包。它会将数据包追加到接收缓冲区，
		*  然后解析RTSP Interleaved帧，提取RTP/RTCP数据包，并推送到Stream中。
		*  
		*  处理流程：
		*  1. 将新数据追加到接收缓冲区
		*  2. 检查缓冲区是否溢出，如果溢出则重置
		*  3. 循环解析RTSP Interleaved帧：
		*     a. 检查Magic Byte（'$'）
		*     b. 读取Channel ID和Length
		*     c. 检查是否有完整的数据包
		*     d. 根据Channel ID判断是RTP还是RTCP
		*     e. 解析RTP/RTCP数据包
		*  4. 移动未处理的数据到缓冲区开头
		*  
		*  RTSP Interleaved帧格式：
		*  - Byte 0: Magic Byte ('$', 0x24)
		*  - Byte 1: Channel ID (0-255)
		*  - Byte 2-3: Length (大端序，2字节)
		*  - Byte 4+: Payload (RTP或RTCP数据)
		*  
		*  Channel ID说明：
		*  - 0: 视频RTP数据
		*  - 1: 视频RTCP数据
		*  - 2: 音频RTP数据（如果有）
		*  - 3: 音频RTCP数据（如果有）
		*  
		*  @param buffer 接收到的数据包，包含RTSP Interleaved帧
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note 数据包可能不完整，需要缓存并等待更多数据
		*  @note 支持RTP和RTCP两种类型的数据包
		*/
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer)  ;

 
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取共享资源类型（Get Share Resource Type）
		*  
		*  该方法用于返回当前生产者的类型。返回值为kProducerTypeRtsp，
		*  表示这是一个RTSP生产者。
		*  
		*  @return 返回ShareResourceType枚举值，表示生产者类型
		*  @note 该方法用于类型识别和路由
		*/
		virtual ShareResourceType ShareResouceType() const { return kProducerTypeRtsp; }
 
	private:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief RTSP Interleaved帧魔术头（RTSP Interleaved Frame Magic Header）
		*  
		*  该结构体定义了RTSP Interleaved帧的头部格式。RTSP Interleaved模式
		*  用于在TCP连接上传输RTP/RTCP数据，避免使用额外的UDP端口。
		*  
		*  帧格式：
		*  - magic_: Magic Byte，固定为'$'（0x24），用于标识帧的开始
		*  - channel_: Channel ID，0-255，用于区分不同的媒体流和控制流
		*  - length_: Payload长度，2字节大端序，表示后续数据的长度
		*  
		*  Channel ID约定：
		*  - 0: 视频RTP数据
		*  - 1: 视频RTCP数据
		*  - 2: 音频RTP数据
		*  - 3: 音频RTCP数据
		*  
		*  @note 该结构体用于解析RTSP Interleaved帧头
		*  @note 帧头总长度为4字节
		*/
		struct RtspMagic {
			uint8_t magic_;    // Magic Byte: '$' (0x24)，用于标识Interleaved帧
			uint8_t channel_;  // Channel ID: 0-255，用于区分RTP/RTCP和音视频
			uint16_t length_;  // Payload长度: 大端序，2字节
		};

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理RTP数据包（Process RTP Packet）
		*  
		*  该方法用于处理解析后的RTP数据包。它会使用RtpVideoFrameAssembler
		*  组装分片的RTP包，重构完整的视频帧，并推送到Stream中。
		*  
		*  处理流程：
		*  1. 使用RtpVideoFrameAssembler组装RTP包
		*  2. 检查是否有完整的视频帧
		*  3. 将完整的视频帧转换为EncodedImage
		*  4. 推送到Stream中
		*  
		*  @param rtp_packet 解析后的RTP数据包
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note RTP包可能被分片，需要组装为完整的视频帧
		*  @note 当前版本的实现已被注释掉（#if 0），可能正在重构中
		*/
		void ProcessRtpPacket(const libmedia_transfer_protocol::RtpPacketReceived& rtp_packet);
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理RTCP数据包（Process RTCP Packet）
		*  
		*  该方法用于处理解析后的RTCP数据包。RTCP用于传输控制信息，
		*  如接收报告、发送报告、反馈等。
		*  
		*  处理流程：
		*  1. 解析RTCP Common Header
		*  2. 根据RTCP类型处理不同的消息
		*  3. 记录日志或更新统计信息
		*  
		*  RTCP类型：
		*  - SR (Sender Report): 发送报告
		*  - RR (Receiver Report): 接收报告
		*  - SDES (Source Description): 源描述
		*  - BYE: 结束会话
		*  - APP: 应用自定义
		*  
		*  @param data RTCP数据包的ArrayView
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note RTCP包用于统计和控制，不包含媒体数据
		*/
		void ProcessRtcpPacket(const rtc::ArrayView<uint8_t>& data);

	private:
		//std::unique_ptr<libmedia_transfer_protocol::RtpVideoFrameAssembler> rtp_video_frame_assembler_; // RTP视频帧组装器（已注释）
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收缓冲区（Receive Buffer）
		*  
		*  该缓冲区用于缓存从TCP连接接收到的数据。由于TCP是流式传输，
		*  数据包可能不完整，需要缓存并等待更多数据。
		*  
		*  @note 缓冲区大小为8MB（kRecvBufferSize）
		*  @note 使用动态分配，在构造函数中创建，在析构函数中释放
		*/
		uint8_t* recv_buffer_;
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收缓冲区当前大小（Receive Buffer Size）
		*  
		*  该变量记录接收缓冲区中当前有效数据的大小。
		*  
		*  @note 初始值为0
		*  @note 每次接收数据后增加，解析完成后减少
		*/
		int32_t recv_buffer_size_;
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收缓冲区最大大小（Receive Buffer Max Size）
		*  
		*  该常量定义了接收缓冲区的最大大小，为8MB。
		*  
		*  @note 如果接收的数据超过此大小，缓冲区将被重置
		*/
		static constexpr int32_t kRecvBufferSize = 1024 * 1024 * 8;
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 视频SSRC（Video SSRC）
		*  
		*  该变量记录视频流的SSRC（Synchronization Source）。
		*  SSRC用于标识RTP流的来源。
		*  
		*  @note 初始值为0，在接收到第一个RTP包后设置
		*/
		uint32_t video_ssrc_;
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 音频SSRC（Audio SSRC）
		*  
		*  该变量记录音频流的SSRC（Synchronization Source）。
		*  SSRC用于标识RTP流的来源。
		*  
		*  @note 初始值为0，在接收到第一个RTP包后设置
		*/
		uint32_t audio_ssrc_;
	};
}


#endif // 
