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
 
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "gb_media_server_log.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_util.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_received.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"

#include "server/stream.h"
#include "producer/rtc_producer.h"
#include "server/session.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/video_rtp_depacketizer_h264.h"
#include "utils/yaml_config.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/receiver_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/psfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sender_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sdes.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/extended_reports.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/bye.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/rtpfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/pli.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "producer/crtmp_producer.h"
namespace gb_media_server {

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RtmpProducer构造函数实现（RtmpProducer Constructor Implementation）
	*  
	*  该构造函数用于初始化RtmpProducer实例。它会调用Producer基类构造函数，
	*  并初始化RTMP相关的数据结构。
	*  
	*  初始化流程：
	*  1. 调用Producer基类构造函数，关联Stream和Session
	*  2. 初始化RTMP解析器（用于解析RTMP协议）
	*  3. 初始化接收缓冲区（用于缓存不完整的数据包）
	*  4. 初始化MPEG解码器（用于解析FLV格式）
	*  
	*  @param stream 指向Stream的共享指针，用于推送媒体数据
	*  @param s 指向Session的共享指针，用于管理会话状态
	*  @note 构造函数会自动创建RTMP解析器和接收缓冲区
	*  @note 注释掉的代码是早期版本的初始化逻辑，已废弃
	*/
	RtmpProducer::RtmpProducer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		:  Producer(  stream, s) 
		 
	{
		// 注释掉的代码是早期版本的初始化逻辑，已废弃
		// 包括：UFrag/Passwd生成、SSRC生成、SDP设置、DTLS初始化等
		// 这些功能已移至RtcProducer中实现
		
		// 当前版本的RTMP生产者只需要基类构造函数即可完成初始化
		// 具体的RTMP解析逻辑在OnRecv方法中实现
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RtmpProducer析构函数实现（RtmpProducer Destructor Implementation）
	*  
	*  该析构函数用于清理RtmpProducer实例。它会释放所有相关资源，
	*  包括RTMP解析器、接收缓冲区等。
	*  
	*  清理流程：
	*  1. 记录日志，标记析构开始
	*  2. 释放RTMP解析器
	*  3. 释放接收缓冲区
	*  4. 清理所有资源
	*  
	*  @note 析构函数会自动调用，不需要手动释放资源
	*  @note 注释掉的代码是早期版本的清理逻辑，已废弃
	*/
	RtmpProducer::~RtmpProducer()
	{
		// 记录日志，标记析构开始
		GBMEDIASERVER_LOG_T_F(LS_INFO);

		// 注释掉的代码是早期版本的清理逻辑，已废弃
		// 包括：断开DTLS信号连接等
		// 这些功能已移至RtcProducer中实现
		 
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RtmpProducer接收数据实现（RtmpProducer On Receive Implementation）
	*  
	*  该方法用于接收来自RTMP客户端的TCP数据包。它会将数据包追加到接收缓冲区，
	*  然后解析RTMP协议，提取音视频数据，并推送到Stream中。
	*  
	*  处理流程（注释掉的代码展示了早期版本的实现）：
	*  1. 将新数据追加到接收缓冲区
	*  2. 读取2字节的payload长度（大端序）
	*  3. 检查是否有完整的数据包
	*  4. 判断数据包类型（RTP或RTCP）
	*  5. 解析对应类型的数据包
	*  6. 移动未处理的数据到缓冲区开头
	*  
	*  @param buffer1 接收到的数据包，包含RTMP协议数据
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 当前版本的实现已被注释掉（#if 0），可能正在重构中
	*  @note 注释掉的代码展示了TCP分包处理、RTP/RTCP解析等逻辑
	*/
	void RtmpProducer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
	{
#if 0
		// 将新数据追加到接收缓冲区
		memcpy(recv_buffer_ + recv_buffer_size_, buffer1.data(), buffer1.size());
		recv_buffer_size_ += buffer1.size();
		
		int32_t   parse_size = 0;

		// 循环解析接收缓冲区中的数据包
		while (recv_buffer_size_ - parse_size > 2)
		{ 
			// 读取2字节的payload长度（大端序）
			int16_t  payload_size = libmedia_transfer_protocol::ByteReader<int16_t>::ReadBigEndian((&recv_buffer_[parse_size]));
			 
			// 检查是否有完整的数据包
			if ((recv_buffer_size_ - parse_size) < (payload_size + 2))
			{
				// 数据不完整，等待下一个包的到来
				break;
			}
			parse_size += 2;  // 跳过长度字段
			
			// 判断数据包类型：RTP或RTCP
			if (libmedia_transfer_protocol::IsRtpPacket(rtc::ArrayView<uint8_t>(recv_buffer_ + parse_size, payload_size)))
			{
				// 解析RTP数据包
				libmedia_transfer_protocol::RtpPacketReceived  rtp_packet_received; 
				bool ret = rtp_packet_received.Parse(recv_buffer_  + parse_size, payload_size);
				if (!ret)
				{ 
					GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed !!! size:" << (recv_buffer_size_ - parse_size);
				}
				else
				{
					// RTP解析成功，处理媒体数据
					if (rtp_packet_received.PayloadType() == 96)
					{
						// 使用MPEG解码器解析媒体数据（已注释）
						//mpeg_decoder_->parse( rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
					} 
				}
			}
			else if (libmedia_transfer_protocol::IsRtcpPacket(rtc::ArrayView<uint8_t>(recv_buffer_ + parse_size, payload_size)))
			{
				// 解析RTCP数据包
				libmedia_transfer_protocol::rtcp::CommonHeader rtcp_block;
				bool ret = rtcp_block.Parse(recv_buffer_ + parse_size, payload_size);
				if (!ret)
				{
					GBMEDIASERVER_LOG(LS_WARNING) << "rtcp parse failed !!!";
				}
			}
			else
			{
				// 未知类型的数据包
				GBMEDIASERVER_LOG(LS_ERROR) << " not know type --> : payload_size: " << payload_size;
			}
			parse_size += payload_size;
 
		}
		
		// 移动未处理的数据到缓冲区开头
		if (recv_buffer_size_ - parse_size > 0)
		{
			recv_buffer_size_ -= parse_size;
			memmove(recv_buffer_, recv_buffer_+ parse_size,recv_buffer_size_);
		}
		else
		{
			recv_buffer_size_ = 0;
			parse_size = 0;
		}
		
#endif // recv_buffer_size_
	}
}
 