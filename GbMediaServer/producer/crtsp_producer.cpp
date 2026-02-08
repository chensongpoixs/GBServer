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
#include "libmedia_transfer_protocol/rtp_video_frame_assembler.h"

#include "server/stream.h"
#include "server/session.h"
#include "producer/crtsp_producer.h"
#include "rtc_base/time_utils.h"

namespace gb_media_server {

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RtspProducer构造函数实现（RtspProducer Constructor Implementation）
	*  
	*  该构造函数用于初始化RtspProducer实例。它会调用Producer基类构造函数，
	*  并初始化RTSP相关的数据结构，包括接收缓冲区和SSRC。
	*  
	*  初始化流程：
	*  1. 调用Producer基类构造函数，关联Stream和Session
	*  2. 创建RTP视频帧组装器（用于组装分片的RTP包，已注释）
	*  3. 分配接收缓冲区（8MB大小）
	*  4. 初始化接收缓冲区大小为0
	*  5. 初始化视频和音频SSRC为0
	*  
	*  @param stream 指向Stream的共享指针，用于推送媒体数据
	*  @param s 指向Session的共享指针，用于管理会话状态
	*  @note 构造函数会自动分配8MB的接收缓冲区
	*  @note RTP视频帧组装器已被注释掉，可能正在重构中
	*  @note 注释掉的代码是早期版本的初始化逻辑，已废弃
	*/
	RtspProducer::RtspProducer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		:  Producer(  stream, s)
		//, rtp_video_frame_assembler_(std::make_unique<libmedia_transfer_protocol::RtpVideoFrameAssembler>(
	//		libmedia_transfer_protocol::RtpVideoFrameAssembler::kH264))  // 创建H.264视频帧组装器（已注释）
		, recv_buffer_(new uint8_t[kRecvBufferSize])  // 分配8MB接收缓冲区
		, recv_buffer_size_(0)  // 初始化缓冲区大小为0
		, video_ssrc_(0)  // 初始化视频SSRC为0
		, audio_ssrc_(0)  // 初始化音频SSRC为0
	{
		// 注释掉的代码是早期版本的初始化逻辑，已废弃
		// 包括：UFrag/Passwd生成、SSRC生成、SDP设置、DTLS初始化等
		// 这些功能已移至RtcProducer中实现
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RtspProducer析构函数实现（RtspProducer Destructor Implementation）
	*  
	*  该析构函数用于清理RtspProducer实例。它会释放所有相关资源，
	*  特别是接收缓冲区。
	*  
	*  清理流程：
	*  1. 记录日志，标记析构开始
	*  2. 检查接收缓冲区是否存在
	*  3. 释放接收缓冲区内存
	*  4. 将指针设置为nullptr
	*  
	*  @note 析构函数会自动调用，不需要手动释放资源
	*  @note 必须释放接收缓冲区，否则会导致内存泄漏
	*/
	RtspProducer::~RtspProducer()
	{
		// 记录日志，标记析构开始
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		
		// 释放接收缓冲区
		if (recv_buffer_)
		{
			delete[] recv_buffer_;
			recv_buffer_ = nullptr;
		}
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RtspProducer接收数据实现（RtspProducer On Receive Implementation）
	*  
	*  该方法用于接收来自RTSP客户端的TCP数据包。它会将数据包追加到接收缓冲区，
	*  然后解析RTSP Interleaved帧，提取RTP/RTCP数据包，并推送到Stream中。
	*  
	*  处理流程：
	*  1. 检查缓冲区是否溢出，如果溢出则重置
	*  2. 将新数据追加到接收缓冲区
	*  3. 循环解析RTSP Interleaved帧：
	*     a. 检查是否有至少4字节（帧头大小）
	*     b. 检查Magic Byte（'$'）
	*     c. 读取Channel ID和Length
	*     d. 检查是否有完整的数据包
	*     e. 根据数据包类型（RTP或RTCP）进行处理
	*     f. 更新parse_size
	*  4. 移动未处理的数据到缓冲区开头
	*  
	*  RTSP Interleaved帧格式：
	*  - Byte 0: Magic Byte ('$', 0x24)
	*  - Byte 1: Channel ID (0-255)
	*  - Byte 2-3: Length (大端序，2字节)
	*  - Byte 4+: Payload (RTP或RTCP数据)
	*  
	*  @param buffer1 接收到的数据包，包含RTSP Interleaved帧
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note 数据包可能不完整，需要缓存并等待更多数据
	*  @note 如果缓冲区溢出，会重置缓冲区并丢弃所有数据
	*/
	void RtspProducer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
	{
		// 检查缓冲区是否溢出
		if (recv_buffer_size_ + buffer1.size() > kRecvBufferSize)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "recv buffer overflow, reset buffer";
			recv_buffer_size_ = 0;
		}

		// 将新数据追加到接收缓冲区
		memcpy(recv_buffer_ + recv_buffer_size_, buffer1.data(), buffer1.size());
		recv_buffer_size_ += buffer1.size();

		int32_t parse_size = 0;

		// 循环解析RTSP Interleaved帧（TCP模式）
		while (recv_buffer_size_ - parse_size >= 4)
		{
			// 检查是否是RTSP Interleaved帧（Magic Byte为'$'）
			if (recv_buffer_[parse_size] != '$')
			{
				// 可能是RTSP协议消息（如SETUP、PLAY等），跳过
				GBMEDIASERVER_LOG(LS_WARNING) << "not RTSP interleaved frame, skip";
				parse_size = recv_buffer_size_;
				break;
			}

			// 读取Magic Header（4字节）
			RtspMagic magic;
			magic.magic_ = recv_buffer_[parse_size];  // Magic Byte: '$'
			magic.channel_ = recv_buffer_[parse_size + 1];  // Channel ID
			magic.length_ = libmedia_transfer_protocol::ByteReader<uint16_t>::ReadBigEndian(
				&recv_buffer_[parse_size + 2]);  // Payload长度（大端序）

			// 检查是否有完整的数据包
			if (recv_buffer_size_ - parse_size < (4 + magic.length_))
			{
				// 数据不完整，等待更多数据
				break;
			}

			parse_size += 4; // 跳过Magic Header

			// 根据Channel判断是RTP还是RTCP
			if (magic.channel_ == 0 || magic.channel_ == 1)
			{
				// RTP packet (channel 0) or RTCP packet (channel 1)
				rtc::ArrayView<uint8_t> packet_data(recv_buffer_ + parse_size, magic.length_);

				// 判断是RTP还是RTCP
				if (libmedia_transfer_protocol::IsRtpPacket(packet_data))
				{
					// 解析RTP数据包
					libmedia_transfer_protocol::RtpPacketReceived rtp_packet;
					if (rtp_packet.Parse(recv_buffer_ + parse_size, magic.length_))
					{
						// 处理RTP数据包
						ProcessRtpPacket(rtp_packet);
						
						// 如果video_ssrc_为0，则设置为当前RTP包的SSRC
						if (video_ssrc_ == 0)
						{
							video_ssrc_ = rtp_packet.Ssrc();
						}
					}
					else
					{
						GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed";
					}
				}
				else if (libmedia_transfer_protocol::IsRtcpPacket(packet_data))
				{
					// 处理RTCP数据包
					ProcessRtcpPacket(packet_data);
				}
				else
				{
					GBMEDIASERVER_LOG(LS_WARNING) << "unknown packet type, channel:" << (int)magic.channel_;
				}
			}
			else
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "unknown RTSP channel:" << (int)magic.channel_;
			}

			parse_size += magic.length_;
		}

		// 移动未处理的数据到缓冲区开头
		if (recv_buffer_size_ - parse_size > 0)
		{
			memmove(recv_buffer_, recv_buffer_ + parse_size, recv_buffer_size_ - parse_size);
			recv_buffer_size_ -= parse_size;
		}
		else
		{
			recv_buffer_size_ = 0;
		}
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理RTP数据包实现（Process RTP Packet Implementation）
	*  
	*  该方法用于处理解析后的RTP数据包。它会使用RtpVideoFrameAssembler
	*  组装分片的RTP包，重构完整的视频帧，并推送到Stream中。
	*  
	*  处理流程（注释掉的代码展示了早期版本的实现）：
	*  1. 使用RtpVideoFrameAssembler插入RTP包
	*  2. 检查是否有完整的视频帧
	*  3. 遍历所有完整的视频帧
	*  4. 将视频帧转换为EncodedImage
	*  5. 设置时间戳、帧类型、宽度、高度等信息
	*  6. 推送到Stream中
	*  
	*  @param rtp_packet 解析后的RTP数据包
	*  @note 该方法在网络线程中调用，需要注意线程安全
	*  @note RTP包可能被分片，需要组装为完整的视频帧
	*  @note 当前版本的实现已被注释掉（#if 0），可能正在重构中
	*/
	void RtspProducer::ProcessRtpPacket(const libmedia_transfer_protocol::RtpPacketReceived& rtp_packet)
	{
		// 使用RtpVideoFrameAssembler组装视频帧（已注释）
		#if 0
		// 插入RTP包，返回完整的视频帧列表
		auto frames = rtp_video_frame_assembler_->InsertPacket(rtp_packet);

		// 遍历所有完整的视频帧
		for (auto& frame : frames)
		{
			if (frame && frame->size() > 0)
			{
				// 转换为EncodedImage
				libmedia_codec::EncodedImage encoded_image;
				encoded_image.SetEncodedData(
					libmedia_codec::EncodedImageBuffer::Create(frame->data(), frame->size()));
				encoded_image.SetTimestamp(frame->Timestamp());
				encoded_image._frameType = frame->FrameType();
				encoded_image._encodedWidth = frame->_encodedWidth;
				encoded_image._encodedHeight = frame->_encodedHeight;

				// 推送到Stream
				GetStream()->AddVideoFrame(std::move(encoded_image));
			}
		}
		#endif // rtp_video_frame_assembler_
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理RTCP数据包实现（Process RTCP Packet Implementation）
	*  
	*  该方法用于处理解析后的RTCP数据包。RTCP用于传输控制信息，
	*  如接收报告、发送报告、反馈等。
	*  
	*  处理流程：
	*  1. 创建RTCP CommonHeader对象
	*  2. 解析RTCP数据包
	*  3. 如果解析成功，记录RTCP类型
	*  4. 如果解析失败，记录警告日志
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
	*  @note 当前版本只记录日志，未进行实际处理
	*/
	void RtspProducer::ProcessRtcpPacket(const rtc::ArrayView<uint8_t>& data)
	{
		// 创建RTCP CommonHeader对象
		libmedia_transfer_protocol::rtcp::CommonHeader rtcp_header;
		
		// 解析RTCP数据包
		if (rtcp_header.Parse(data.data(), data.size()))
		{
			// RTCP包处理（可以用于统计、同步等）
			GBMEDIASERVER_LOG(LS_VERBOSE) << "received RTCP packet, type:" << (int)rtcp_header.type();
		}
		else
		{
			// 解析失败
			GBMEDIASERVER_LOG(LS_WARNING) << "rtcp parse failed";
		}
	}
}

  