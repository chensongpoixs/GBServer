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


#ifndef _C_GB_MEDIA_SERVER_GB28181_PUSH_Producer_H_
#define _C_GB_MEDIA_SERVER_GB28181_PUSH_Producer_H_

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



namespace gb_media_server {

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief GB28181生产者类（GB28181 Producer）
	*  
	*  Gb28181Producer是GBMediaServer流媒体服务器中用于GB28181协议推流的生产者类。
	*  该类继承自Producer基类，负责接收来自GB28181设备的PS流（Program Stream），
	*  解析PS流中的音视频数据，并将其推送到服务器的媒体流中。
	*  
	*  GB28181协议说明：
	*  - GB28181是中国公共安全视频监控联网系统的国家标准
	*  - GB28181使用SIP协议进行信令交互，使用RTP协议传输媒体数据
	*  - 媒体数据封装为PS流（Program Stream），基于MPEG-2标准
	*  - PS流包含视频流（通常为H264）和音频流（通常为G711或AAC）
	*  
	*  PS流格式说明：
	*  - PS流由多个PS包组成，每个PS包包含一个或多个PES包
	*  - PES包（Packetized Elementary Stream）包含音频或视频的基本流数据
	*  - PS包头包含时间戳、流ID等信息
	*  - 视频流ID通常为0xE0，音频流ID通常为0xC0
	*  
	*  工作流程：
	*  1. GB28181设备通过SIP协议注册到服务器
	*  2. 服务器发送INVITE请求，设备开始推流
	*  3. 设备通过RTP协议发送PS流数据
	*  4. Gb28181Producer接收RTP包，提取PS流数据
	*  5. 使用MpegDecoder解析PS流，提取音视频帧
	*  6. 将音视频帧推送到Stream中，供消费者使用
	*  
	*  @note 该类使用TCP interleaved模式接收RTP包
	*  @note PS流解析使用MpegDecoder类，支持H264视频和G711/AAC音频
	*  @note 接收缓冲区大小为8MB，用于处理大帧和网络抖动
	*  
	*  使用示例：
	*  @code
	*  // 在GB28181服务中创建GB28181生产者
	*  auto producer = std::make_shared<Gb28181Producer>(stream, session);
	*  session->SetProducer(producer);
	*  // Gb28181Producer会自动处理后续的PS流解析和媒体帧推送
	*  @endcode
	*/
	class Gb28181Producer : public Producer, public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 构造GB28181生产者（Constructor）
		*  
		*  该构造函数用于创建Gb28181Producer实例。它会初始化MPEG解码器、
		*  接收缓冲区，并连接信号槽用于接收解析后的音视频帧。
		*  
		*  初始化流程：
		*  1. 调用基类Producer的构造函数，初始化流和会话
		*  2. 创建MpegDecoder实例，用于解析PS流
		*  3. 分配接收缓冲区（8MB），用于存储接收到的RTP数据
		*  4. 连接MpegDecoder的信号槽：
		*     - SignalRecvVideoFrame：接收解析后的视频帧
		*     - SignalRecvAudioFrame：接收解析后的音频帧
		*  
		*  @param stream 流对象的共享指针，用于推送媒体流数据
		*  @param s 会话对象的共享指针，用于管理会话生命周期
		*  @note 接收缓冲区大小为8MB，可根据需要调整
		*  @note MpegDecoder会自动解析PS流并触发信号槽回调
		*  
		*  使用示例：
		*  @code
		*  auto producer = std::make_shared<Gb28181Producer>(stream, session);
		*  @endcode
		*/
		explicit Gb28181Producer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 析构GB28181生产者（Destructor）
		*  
		*  该析构函数用于清理Gb28181Producer实例。它会释放MPEG解码器、
		*  接收缓冲区等资源。
		*  
		*  清理流程：
		*  1. 断开MpegDecoder的信号槽连接
		*  2. 释放MpegDecoder实例
		*  3. 释放接收缓冲区内存
		*  4. 调用基类的析构函数进行基类资源清理
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		virtual ~Gb28181Producer();
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收数据（On Receive）
		*  
		*  该方法用于接收来自GB28181设备的RTP数据。数据通过TCP interleaved模式传输，
		*  每个RTP包前面有2字节的长度字段。
		*  
		*  数据处理流程：
		*  1. 将接收到的数据追加到接收缓冲区
		*  2. 循环解析缓冲区中的数据包：
		*     - 读取2字节的长度字段（大端序）
		*     - 检查是否有完整的数据包
		*     - 判断数据包类型（RTP或RTCP）
		*     - 如果是RTP包，解析RTP头部并提取payload
		*     - 如果payload type为96（PS流），传递给MpegDecoder解析
		*     - 如果是RTCP包，解析RTCP头部（当前只记录日志）
		*  3. 移动未处理的数据到缓冲区开头
		*  
		*  RTP包格式：
		*  - Length（2字节）：RTP包的长度（大端序）
		*  - RTP Header（12字节）：包含版本、payload type、序列号、时间戳、SSRC等
		*  - RTP Payload（N字节）：PS流数据
		*  
		*  @param buffer 接收到的数据缓冲区，包含一个或多个RTP包
		*  @note 该方法会在网络线程中调用，需要注意线程安全
		*  @note 接收缓冲区会自动管理，防止溢出
		*  @note PS流的payload type通常为96
		*  
		*  使用示例：
		*  @code
		*  // 在网络线程中调用
		*  producer->OnRecv(received_buffer);
		*  @endcode
		*/
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer)  ;

	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取资源类型（Get Share Resource Type）
		*  
		*  该方法用于标识该生产者的资源类型。对于Gb28181Producer，返回kProducerTypeGB28181。
		*  
		*  @return 返回kProducerTypeGB28181，表示这是GB28181协议的生产者
		*/
		virtual ShareResourceType ShareResouceType() const { return kProducerTypeGB28181; }
		
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理视频帧（On Process Video Frame）
		*  
		*  该方法是MpegDecoder的信号槽回调，当解析出完整的视频帧时触发。
		*  视频帧通常为H264格式，包含完整的NALU单元。
		*  
		*  处理流程：
		*  1. 接收MpegDecoder解析出的视频帧
		*  2. 将视频帧推送到Stream中
		*  3. Stream会将视频帧分发给所有消费者
		*  
		*  @param frame 编码后的视频帧，包含H264编码数据和元信息（时间戳、尺寸等）
		*  @note 该方法会在MpegDecoder解析线程中调用
		*  @note 视频帧会被移动（std::move）到Stream中，避免拷贝
		*  @note 可以通过取消注释的代码将视频帧保存到文件，用于调试
		*  
		*  使用示例：
		*  @code
		*  // 该方法由MpegDecoder自动调用，不需要手动调用
		*  @endcode
		*/
		void OnProcessVideoFrame(libmedia_codec::EncodedImage  frame);
		
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理音频帧（On Process Audio Frame）
		*  
		*  该方法是MpegDecoder的信号槽回调，当解析出完整的音频帧时触发。
		*  音频帧通常为G711或AAC格式。
		*  
		*  处理流程：
		*  1. 接收MpegDecoder解析出的音频帧
		*  2. 将音频帧推送到Stream中（当前代码中被注释掉）
		*  3. Stream会将音频帧分发给所有消费者
		*  
		*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据
		*  @param pts 呈现时间戳（Presentation Time Stamp），单位为毫秒
		*  @note 该方法会在MpegDecoder解析线程中调用
		*  @note 当前实现中音频处理被注释掉，需要根据实际需求启用
		*  @note 可以通过取消注释的代码将音频帧保存到文件，用于调试
		*  
		*  使用示例：
		*  @code
		*  // 该方法由MpegDecoder自动调用，不需要手动调用
		*  @endcode
		*/
		void OnProcessAudioFrame( rtc::CopyOnWriteBuffer   frame, int64_t  pts);
		
	private:
		/**
		*  @brief MPEG解码器
		*  
		*  该对象用于解析PS流（Program Stream），提取音视频帧。
		*  MpegDecoder会自动识别PS包头、PES包头，并提取基本流数据。
		*/
		std::unique_ptr< libmedia_transfer_protocol::libmpeg::MpegDecoder>    mpeg_decoder_;

		/**
		*  @brief 接收缓冲区指针
		*  
		*  该缓冲区用于存储接收到的RTP数据，大小为8MB。
		*  缓冲区会自动管理，当数据处理完成后会移动未处理的数据到开头。
		*/
		uint8_t  * recv_buffer_;
		
		/**
		*  @brief 接收缓冲区当前大小
		*  
		*  该变量记录接收缓冲区中当前存储的数据大小。
		*  当接收到新数据时，会追加到缓冲区末尾并更新该变量。
		*/
		int32_t recv_buffer_size_;
	};
}


#endif // 