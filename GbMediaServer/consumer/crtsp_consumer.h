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
 /***********************************************************************************************
 created: 		2025-04-29

 author:			chensong

 purpose:		rtsp_consumer
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
 ************************************************************************************************/

#ifndef _C_RTSP_CONSUMER_
#define _C_RTSP_CONSUMER_
#include "consumer/consumer.h"

#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "rtc_base/copy_on_write_buffer.h"
#include <memory>

namespace gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-04-29
	*  @brief RTSP消费者类（RTSP Consumer）
	*  
	*  RtspConsumer是GBMediaServer流媒体服务器中用于RTSP协议播放的消费者类。
	*  该类继承自Consumer基类，负责将接收到的媒体流（视频和音频）封装为RTP包
	*  并通过RTSP interleaved模式（TCP）或UDP模式发送给客户端进行播放。
	*  
	*  RTSP（Real Time Streaming Protocol）协议说明：
	*  - RTSP是一种应用层协议，用于控制流媒体服务器的播放、暂停、停止等操作
	*  - RTSP本身不传输媒体数据，媒体数据通过RTP/RTCP协议传输
	*  - RTSP支持两种传输模式：TCP interleaved模式和UDP模式
	*  - TCP interleaved模式：RTP/RTCP数据通过RTSP的TCP连接传输，使用特殊的帧格式
	*  - UDP模式：RTP/RTCP数据通过独立的UDP端口传输
	*  
	*  RTSP Interleaved帧格式：
	*  - Magic Byte（1字节）：固定为'$'（0x24）
	*  - Channel ID（1字节）：通道标识，0表示RTP，1表示RTCP
	*  - Length（2字节）：RTP/RTCP包的长度（大端序）
	*  - Data（N字节）：RTP/RTCP包数据
	*  
	*  工作流程：
	*  1. 客户端通过RTSP协议连接服务器，发送DESCRIBE、SETUP、PLAY等命令
	*  2. 服务器创建RtspConsumer实例，关联网络连接
	*  3. RtspConsumer初始化RTP封装器和RTCP上下文
	*  4. 当接收到视频/音频帧时，封装为RTP包并通过interleaved模式发送
	*  5. 定期发送RTCP包，用于同步和质量反馈
	*  6. 持续发送媒体帧，直到客户端发送TEARDOWN命令
	*  
	*  @note 该类实现单播模式的RTSP流媒体分发
	*  @note 当前实现使用TCP interleaved模式，RTP和RTCP通过RTSP连接传输
	*  @note RTP时间戳使用90kHz时钟（视频），音频根据采样率确定
	*  
	*  使用示例：
	*  @code
	*  // 在RTSP服务中创建RTSP消费者
	*  auto consumer = session->CreateConsumer(connection, "live/stream1", "", ShareResourceType::kConsumerTypeRtsp);
	*  // RtspConsumer会自动处理后续的RTP封装和发送
	*  @endcode
	*/
	class RtspConsumer : public  Consumer
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 构造RTSP消费者（Constructor）
		*  
		*  该构造函数用于创建RtspConsumer实例。RTSP消费者负责将媒体流封装为RTP包
		*  并通过RTSP interleaved模式发送给客户端。
		*  
		*  初始化流程：
		*  1. 调用基类Consumer的构造函数，初始化流和会话
		*  2. 保存网络连接指针，用于后续数据发送
		*  3. 初始化RTP header extension map，用于RTP扩展头
		*  4. 初始化视频SSRC、序列号、时间戳和payload type
		*  5. 注册RTP扩展头类型（传输时间偏移、绝对发送时间等）
		*  
		*  @param connection 网络连接对象指针，用于与客户端通信，不能为空
		*  @param stream 流对象的共享指针，用于访问媒体流数据
		*  @param s 会话对象的共享指针，用于管理会话生命周期
		*  @note 网络连接对象必须在RtspConsumer生命周期内保持有效
		*  @note SSRC（Synchronization Source）用于标识RTP流的源
		*  @note 序列号从0开始递增，用于检测丢包和重排序
		*  
		*  使用示例：
		*  @code
		*  auto consumer = std::make_shared<RtspConsumer>(connection, stream, session);
		*  @endcode
		*/
		explicit RtspConsumer(libmedia_transfer_protocol::libnetwork::Connection*    connection, 
			std::shared_ptr<Stream> &stream, const std::shared_ptr<Session> &s);
		
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 析构RTSP消费者（Destructor）
		*  
		*  该析构函数用于清理RtspConsumer实例。主要清理RTP封装器和相关资源，
		*  其他资源的清理由基类负责。
		*  
		*  清理流程：
		*  1. 清理RTP header extension map
		*  2. 基类析构函数自动清理其他资源
		*  
		*  @note 析构时不需要手动关闭网络连接，连接的生命周期由外部管理
		*/
		virtual ~RtspConsumer();

	public:
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 处理视频帧（On Video Frame）
		*  
		*  该方法用于处理接收到的视频帧，将视频帧封装为RTP包
		*  并通过RTSP interleaved模式发送给客户端。
		*  
		*  视频帧处理流程：
		*  1. 检查视频帧是否有效（大小大于0）且网络连接存在
		*  2. 调用PacketizeH264Frame方法将H264帧封装为RTP包
		*  3. RTP包通过SendRtpPacketInterleaved方法发送
		*  
		*  RTP封装说明：
		*  - H264视频帧可能被分割为多个RTP包（分片模式）
		*  - 每个RTP包包含RTP头部和H264 payload
		*  - RTP头部包含序列号、时间戳、SSRC、marker bit等
		*  - 最后一个RTP包的marker bit设置为1，表示帧结束
		*  
		*  @param frame 编码后的视频帧，包含H264编码数据和元信息（时间戳、尺寸等）
		*  @note 该方法会自动处理H264帧的RTP封装和分片
		*  @note 视频时间戳使用90kHz时钟（RTP标准）
		*  @note 如果网络连接不存在，会直接返回
		*  
		*  使用示例：
		*  @code
		*  libmedia_codec::EncodedImage frame;
		*  frame.SetTimestamp(timestamp_ms);
		*  frame.SetEncodedData(encoded_data);
		*  rtsp_consumer->OnVideoFrame(frame);
		*  @endcode
		*/
		virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame);
		
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 处理音频帧（On Audio Frame）
		*  
		*  该方法用于处理接收到的音频帧，将音频帧封装为RTP包
		*  并通过RTSP interleaved模式发送给客户端。
		*  
		*  音频帧处理流程：
		*  1. 记录音频帧接收日志（当前实现）
		*  2. 将音频帧封装为RTP包（待实现）
		*  3. RTP包通过SendRtpPacketInterleaved方法发送
		*  
		*  音频RTP封装说明：
		*  - 音频帧通常较小，一个RTP包可以包含一个完整的音频帧
		*  - RTP头部包含序列号、时间戳、SSRC、marker bit等
		*  - 音频时间戳根据采样率计算（例如48kHz采样率）
		*  
		*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据
		*  @param pts 呈现时间戳（Presentation Time Stamp），单位为毫秒
		*  @note 当前实现只记录日志，音频RTP封装待实现
		*  @note 音频时间戳需要根据采样率转换为RTP时间戳
		*  
		*  使用示例：
		*  @code
		*  rtc::CopyOnWriteBuffer audio_data(aac_encoded_data, data_size);
		*  int64_t pts_ms = rtc::TimeMillis();
		*  rtsp_consumer->OnAudioFrame(audio_data, pts_ms);
		*  @endcode
		*/
		virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);
		
	public:
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 获取资源类型（Get Resource Type）
		*  
		*  该方法用于标识消费者的资源类型，返回RTSP消费者类型标识。
		*  资源类型用于系统区分不同的消费者实现类。
		*  
		*  @return 返回 ShareResourceType::kConsumerTypeRtsp，表示这是RTSP协议的消费者
		*  @note 该方法为虚函数重写，用于运行时类型识别
		*  
		*  使用示例：
		*  @code
		*  ShareResourceType type = rtsp_consumer->ShareResouceType();
		*  // type == ShareResourceType::kConsumerTypeRtsp
		*  @endcode
		*/
		virtual ShareResourceType ShareResouceType() const { return kConsumerTypeRtsp; }

	private:
		/**
		*  @brief 发送RTSP interleaved RTP包
		*  
		*  该方法将RTP包封装为RTSP interleaved帧格式并通过TCP连接发送。
		*  
		*  Interleaved帧格式：
		*  - Magic Byte（1字节）：固定为'$'（0x24）
		*  - Channel ID（1字节）：0表示RTP，1表示RTCP
		*  - Length（2字节）：RTP包的长度（大端序）
		*  - Data（N字节）：RTP包数据
		*  
		*  @param rtp_packet RTP包对象，包含完整的RTP头部和payload
		*  @param channel 通道ID，0表示RTP，1表示RTCP
		*  @note 该方法会自动添加interleaved帧头部
		*  @note 如果网络连接不存在，会直接返回
		*/
		void SendRtpPacketInterleaved(const libmedia_transfer_protocol::RtpPacketToSend& rtp_packet, uint8_t channel);
		
		/**
		*  @brief 封装H264视频帧为RTP包
		*  
		*  该方法将H264视频帧封装为一个或多个RTP包。如果视频帧较大，
		*  会使用分片模式（Fragmentation Unit）将帧分割为多个RTP包。
		*  
		*  封装流程：
		*  1. 创建RTP packetizer，设置payload大小限制
		*  2. 创建RTP video header，设置H264相关参数
		*  3. 更新RTP时间戳（90kHz时钟）
		*  4. 循环生成RTP包，直到所有数据封装完成
		*  5. 设置最后一个包的marker bit为1
		*  6. 通过SendRtpPacketInterleaved发送每个RTP包
		*  
		*  @param frame 编码后的H264视频帧
		*  @note 该方法会自动处理H264帧的分片
		*  @note 序列号会自动递增
		*  @note 时间戳使用90kHz时钟
		*/
		void PacketizeH264Frame(const libmedia_codec::EncodedImage& frame);

	private:
		/**
		*  @brief 网络连接对象指针
		*  
		*  该指针指向与客户端建立的TCP连接对象，用于发送RTSP interleaved帧。
		*  连接对象的生命周期由外部管理，RtspConsumer只持有指针。
		*/
		libmedia_transfer_protocol::libnetwork::Connection*    connection_;
		
		/**
		*  @brief RTP头部扩展映射表
		*  
		*  该对象用于管理RTP头部扩展，支持传输时间偏移、绝对发送时间等扩展。
		*  RTP扩展头用于传输额外的元数据，例如时间戳、序列号等。
		*/
		std::unique_ptr<libmedia_transfer_protocol::RtpHeaderExtensionMap> rtp_header_extension_map_;
		
		/**
		*  @brief 视频SSRC（Synchronization Source）
		*  
		*  SSRC用于标识RTP流的源，每个媒体流有唯一的SSRC。
		*  默认值为0x12345678，可以根据需要修改。
		*/
		uint32_t video_ssrc_;
		
		/**
		*  @brief 视频RTP序列号
		*  
		*  RTP序列号从0开始递增，用于检测丢包和重排序。
		*  每发送一个RTP包，序列号加1。
		*/
		uint16_t video_sequence_number_;
		
		/**
		*  @brief 视频RTP时间戳
		*  
		*  RTP时间戳使用90kHz时钟（视频标准），用于同步和播放控制。
		*  时间戳根据视频帧的呈现时间计算。
		*/
		uint32_t video_rtp_timestamp_;
		
		/**
		*  @brief 视频payload type
		*  
		*  Payload type用于标识RTP包中的媒体类型，H264通常使用96。
		*  该值在SDP协商时确定。
		*/
		uint8_t video_payload_type_;
		
		/**
		*  @brief RTP通道ID（用于RTSP interleaved模式）
		*  
		*  在RTSP interleaved模式下，RTP数据使用通道0传输。
		*/
		static constexpr uint8_t kRtpChannel = 0;
		
		/**
		*  @brief RTCP通道ID（用于RTSP interleaved模式）
		*  
		*  在RTSP interleaved模式下，RTCP数据使用通道1传输。
		*/
		static constexpr uint8_t kRtcpChannel = 1;
	};
}

#endif // _C_RTSP_CONSUMER_