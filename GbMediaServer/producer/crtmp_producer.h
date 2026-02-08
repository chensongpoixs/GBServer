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


#ifndef _C_GB_MEDIA_SERVER_RTMP_Producer_H_
#define _C_GB_MEDIA_SERVER_RTMP_Producer_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
 
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h" 
#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 


#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/libmpeg/mpeg_decoder.h"
#include "producer/producer.h"
#include "share/rtc_interface.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_context_recv.h"

#include "libmedia_codec/video_codecs/nal_parse_factory.h"

namespace gb_media_server {

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTMP生产者类（RTMP Producer）
	*  
	*  RtmpProducer是GBMediaServer流媒体服务器中用于处理RTMP推流的生产者类。
	*  它负责接收来自RTMP客户端的媒体数据，解析RTMP协议，提取音视频数据，
	*  并将这些数据推送到Stream中，供消费者使用。
	*  
	*  RTMP协议说明：
	*  - RTMP（Real-Time Messaging Protocol）是Adobe开发的流媒体传输协议
	*  - RTMP使用TCP传输，默认端口1935
	*  - RTMP数据格式为FLV（Flash Video）格式
	*  - RTMP支持音频（AAC、MP3等）和视频（H.264、H.265等）编码
	*  
	*  工作流程：
	*  1. 接收来自RTMP客户端的TCP数据包
	*  2. 解析RTMP协议头（Chunk Header）
	*  3. 提取RTMP消息（Message）
	*  4. 解析FLV格式的音视频数据
	*  5. 将音视频数据推送到Stream中
	*  
	*  数据格式：
	*  - RTMP Chunk: 包含Chunk Header和Chunk Data
	*  - RTMP Message: 包含Message Header和Message Payload
	*  - FLV Tag: 包含Tag Header和Tag Data（音频/视频/脚本数据）
	*  
	*  @note RtmpProducer继承自Producer和sigslot::has_slots<>
	*  @note 支持RTMP推流协议（RTMP、RTMPS、RTMPT等）
	*  @note 支持多种音视频编码格式（H.264、AAC等）
	*  
	*  使用示例：
	*  @code
	*  // 创建RTMP生产者
	*  auto producer = std::make_shared<RtmpProducer>(stream, session);
	*  
	*  // 接收RTMP数据
	*  producer->OnRecv(buffer);
	*  @endcode
	*/
	class RtmpProducer :    public Producer, public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化RtmpProducer实例。它会创建RTMP解析器，
		*  初始化接收缓冲区，并准备接收RTMP数据。
		*  
		*  初始化流程：
		*  1. 调用Producer基类构造函数，关联Stream和Session
		*  2. 创建RTMP解析器（用于解析RTMP协议）
		*  3. 初始化接收缓冲区（用于缓存不完整的数据包）
		*  4. 初始化MPEG解码器（用于解析FLV格式）
		*  
		*  @param stream 指向Stream的共享指针，用于推送媒体数据
		*  @param s 指向Session的共享指针，用于管理会话状态
		*  @note 构造函数会自动创建RTMP解析器和接收缓冲区
		*/
		explicit RtmpProducer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtmpProducer实例。它会释放所有相关资源，
		*  包括RTMP解析器、接收缓冲区等。
		*  
		*  清理流程：
		*  1. 停止接收RTMP数据
		*  2. 释放RTMP解析器
		*  3. 释放接收缓冲区
		*  4. 清理所有资源
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual ~RtmpProducer();
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收数据（On Receive）
		*  
		*  该方法用于接收来自RTMP客户端的TCP数据包。它会将数据包追加到接收缓冲区，
		*  然后解析RTMP协议，提取音视频数据，并推送到Stream中。
		*  
		*  处理流程：
		*  1. 将新数据追加到接收缓冲区
		*  2. 解析RTMP Chunk Header（包含Chunk Type、Stream ID等）
		*  3. 提取RTMP Message（包含Message Type、Timestamp等）
		*  4. 根据Message Type解析数据（音频/视频/脚本数据）
		*  5. 将音视频数据推送到Stream中
		*  6. 移动未处理的数据到缓冲区开头
		*  
		*  RTMP协议解析：
		*  - Chunk Type 0: 完整的Chunk Header（11字节）
		*  - Chunk Type 1: 不包含Message Stream ID（7字节）
		*  - Chunk Type 2: 只包含Timestamp Delta（3字节）
		*  - Chunk Type 3: 不包含任何Header（0字节）
		*  
		*  @param buffer 接收到的数据包，包含RTMP协议数据
		*  @note 该方法在网络线程中调用，需要注意线程安全
		*  @note 数据包可能不完整，需要缓存并等待更多数据
		*  @note 支持RTMP协议的所有Chunk Type
		*/
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer)  ;

 
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取共享资源类型（Get Share Resource Type）
		*  
		*  该方法用于返回当前生产者的类型。返回值为kProducerTypeRtmp，
		*  表示这是一个RTMP生产者。
		*  
		*  @return 返回ShareResourceType枚举值，表示生产者类型
		*  @note 该方法用于类型识别和路由
		*/
		virtual ShareResourceType ShareResouceType() const { return kProducerTypeRtmp; }
 
	private:
		// 私有成员变量（如果有的话）
		// 例如：RTMP解析器、接收缓冲区、MPEG解码器等

	private:
		// 私有方法（如果有的话）
		// 例如：解析RTMP Chunk、解析FLV Tag等
 
		
	};
}


#endif // 