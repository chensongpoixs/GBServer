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

 purpose:		http_parser
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

#ifndef _C_FLV_CONSUMER_
#define _C_FLV_CONSUMER_
#include "consumer/consumer.h"

#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "server/session.h"
namespace gb_media_server
{
	class Session;
	/**
	*  @author chensong
	*  @date 2025-04-29
	*  @brief FLV消费者类（FLV Consumer）
	*  
	*  FlvConsumer是GBMediaServer流媒体服务器中用于HTTP-FLV协议播放的消费者类。
	*  该类继承自Consumer基类，负责将接收到的媒体流（视频和音频）封装为FLV格式
	*  并通过HTTP连接发送给客户端进行播放。
	*  
	*  FLV（Flash Video）协议说明：
	*  - FLV是Adobe开发的视频容器格式，广泛用于流媒体传输
	*  - HTTP-FLV是基于HTTP协议的FLV流媒体传输方式
	*  - 支持实时流媒体播放，延迟较低，适合直播场景
	*  - 客户端可以通过HTTP请求直接拉取FLV流进行播放
	*  
	*  FLV格式结构：
	*  - FLV Header（9字节）：包含"FLV"标识、版本号、标志位等
	*  - FLV Body：包含多个Tag（标签），每个Tag包含一个媒体帧
	*  - Tag类型：Script Tag（元数据）、Video Tag（视频帧）、Audio Tag（音频帧）
	*  
	*  工作流程：
	*  1. 客户端通过HTTP请求连接服务器，请求FLV流
	*  2. 服务器创建FlvConsumer实例，关联网络连接
	*  3. FlvConsumer初始化FLV编码器上下文
	*  4. 当接收到视频/音频帧时，封装为FLV Tag并发送
	*  5. 首次发送视频/音频帧前，先发送FLV Header
	*  6. 持续发送媒体帧，直到客户端断开连接
	*  
	*  @note 该类实现单播模式的FLV流媒体分发
	*  @note FLV时间戳精度为毫秒级别，需要转换微秒级时间戳
	*  @note 该类自动管理FLV Header的发送，确保只发送一次
	*  
	*  使用示例：
	*  @code
	*  // 在WebService中创建FLV消费者
	*  auto consumer = session->CreateConsumer(connection, "live/stream1", "", ShareResourceType::kConsumerTypeFlv);
	*  // FlvConsumer会自动处理后续的媒体帧封装和发送
	*  @endcode
	*/
	class FlvConsumer : public  Consumer
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 构造FLV消费者（Constructor）
		*  
		*  该构造函数用于创建FLV消费者实例。FLV消费者负责将媒体流封装为FLV格式
		*  并通过HTTP连接发送给客户端。
		*  
		*  初始化流程：
		*  1. 调用基类Consumer的构造函数，初始化流和会话
		*  2. 保存网络连接指针，用于后续数据发送
		*  3. 初始化FLV Header发送标志为false
		*  
		*  @param connection 网络连接对象指针，用于与客户端通信，不能为空
		*  @param stream 流对象的共享指针，用于访问媒体流数据
		*  @param s 会话对象的共享指针，用于管理会话生命周期
		*  @note 网络连接对象必须在FlvConsumer生命周期内保持有效
		*  @note FLV编码器上下文需要在连接建立后单独设置
		*  
		*  使用示例：
		*  @code
		*  auto consumer = std::make_shared<FlvConsumer>(connection, stream, session);
		*  @endcode
		*/
		explicit FlvConsumer(libmedia_transfer_protocol::libnetwork::Connection*    connection, 
			std::shared_ptr<Stream> &stream, const std::shared_ptr<Session> &s);

		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 析构FLV消费者（Destructor）
		*  
		*  该析构函数用于清理FLV消费者实例。主要重置FLV Header发送标志，
		*  其他资源的清理由基类负责。
		*  
		*  清理流程：
		*  1. 重置FLV Header发送标志为false
		*  2. 基类析构函数自动清理其他资源
		*  
		*  @note 析构时不需要手动关闭网络连接，连接的生命周期由外部管理
		*/
		virtual ~FlvConsumer();

	public:
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 处理视频帧（On Video Frame）
		*  
		*  该方法用于处理接收到的视频帧，将视频帧封装为FLV视频标签（Video Tag）
		*  并通过网络连接发送给客户端。
		*  
		*  视频帧处理流程：
		*  1. 检查视频帧是否有效（大小大于0）
		*  2. 从网络连接上下文中获取FLV编码器
		*  3. 如果是首次发送，先发送FLV Header（包含视频和音频标志）
		*  4. 将视频帧封装为FLV Video Tag并发送
		*  5. 时间戳转换为毫秒级别（FLV要求）
		*  
		*  FLV Video Tag结构：
		*  - Tag Header（11字节）：包含Tag类型、数据大小、时间戳等
		*  - Video Tag Body：包含视频编码格式（H264）、帧类型（关键帧/非关键帧）和编码数据
		*  - Previous Tag Size（4字节）：前一个Tag的大小
		*  
		*  @param frame 编码后的视频帧，包含H264编码数据和元信息（时间戳、尺寸等）
		*  @note 该方法会在首次调用时自动发送FLV Header
		*  @note 视频帧时间戳会被转换为毫秒级别（除以1000）
		*  @note 如果FLV编码器上下文不存在，会记录警告日志并返回
		*  
		*  使用示例：
		*  @code
		*  libmedia_codec::EncodedImage frame;
		*  frame.SetTimestamp(timestamp_us);
		*  frame.SetEncodedData(encoded_data);
		*  flv_consumer->OnVideoFrame(frame);
		*  @endcode
		*/
		virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame);

		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 处理音频帧（On Audio Frame）
		*  
		*  该方法用于处理接收到的音频帧，将音频帧封装为FLV音频标签（Audio Tag）
		*  并通过网络连接发送给客户端。
		*  
		*  音频帧处理流程：
		*  1. 从网络连接上下文中获取FLV编码器
		*  2. 如果是首次发送，先发送FLV Header（包含视频和音频标志）
		*  3. 将音频帧封装为FLV Audio Tag并发送
		*  4. 时间戳转换为毫秒级别（FLV要求）
		*  
		*  FLV Audio Tag结构：
		*  - Tag Header（11字节）：包含Tag类型、数据大小、时间戳等
		*  - Audio Tag Body：包含音频编码格式（AAC）、采样率、声道数和编码数据
		*  - Previous Tag Size（4字节）：前一个Tag的大小
		*  
		*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据（AAC格式）
		*  @param pts 呈现时间戳（Presentation Time Stamp），单位为微秒
		*  @note 该方法会在首次调用时自动发送FLV Header（如果视频帧未先到达）
		*  @note 音频时间戳会被转换为毫秒级别（除以1000）
		*  @note 如果FLV编码器上下文不存在，会记录警告日志并返回
		*  @note FLV的PTS精度为毫秒级别，需要从微秒转换
		*  
		*  使用示例：
		*  @code
		*  rtc::CopyOnWriteBuffer audio_data(aac_encoded_data, data_size);
		*  int64_t pts_us = rtc::TimeMicros(); // 微秒级时间戳
		*  flv_consumer->OnAudioFrame(audio_data, pts_us);
		*  @endcode
		*/
		virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);

	public:
		/**
		*  @author chensong
		*  @date 2025-04-29
		*  @brief 获取资源类型（Get Resource Type）
		*  
		*  该方法用于标识消费者的资源类型，返回FLV消费者类型标识。
		*  资源类型用于系统区分不同的消费者实现类。
		*  
		*  @return 返回 ShareResourceType::kConsumerTypeFlv，表示这是FLV协议的消费者
		*  @note 该方法为虚函数重写，用于运行时类型识别
		*  
		*  使用示例：
		*  @code
		*  ShareResourceType type = flv_consumer->ShareResouceType();
		*  // type == ShareResourceType::kConsumerTypeFlv
		*  @endcode
		*/
		virtual ShareResourceType ShareResouceType() const { return kConsumerTypeFlv; }

	private:
		libmedia_transfer_protocol::libnetwork::Connection*    connection_;


		bool													send_flv_header_{false};
	};
}

#endif // _C_FLV_CONSUMER_
