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
#include "consumer/flv_consumer.h"
#include "libmedia_transfer_protocol/libflv/cflv_encoder.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "gb_media_server_log.h"
namespace gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief FLV消费者构造函数（FLV Consumer Constructor）
	*  
	*  该构造函数用于创建FLV消费者实例，初始化HTTP连接、流对象和会话对象。
	*  FLV消费者负责将媒体流封装为FLV格式，通过HTTP连接发送给客户端。
	*  
	*  初始化流程：
	*  1. 调用基类Consumer的构造函数，传入流对象和会话对象
	*  2. 保存HTTP连接对象指针，用于发送FLV数据
	*  3. 设置send_flv_header_标志为false，表示尚未发送FLV头部
	*  4. 注释掉的代码用于测试本地采集功能
	*  
	*  FLV格式说明：
	*  - FLV（Flash Video）是Adobe公司推出的流媒体格式
	*  - FLV文件由FLV头部和FLV标签（Tag）组成
	*  - FLV头部包含版本、音频标志、视频标志等信息
	*  - FLV标签包含音频标签、视频标签和脚本标签
	*  - FLV标签包含时间戳、数据大小、数据内容等信息
	*  
	*  @param connection HTTP连接对象指针，用于发送FLV数据
	*  @param stream 流对象的共享指针，用于获取媒体流
	*  @param s 会话对象的共享指针，用于管理会话状态
	*  @note HTTP连接对象由外部管理，不需要在此释放
	*  @note FLV头部会在第一次发送音视频帧时自动发送
	*/
	FlvConsumer::FlvConsumer(libmedia_transfer_protocol::libnetwork::Connection*    connection,
		std::shared_ptr<Stream>& stream, const std::shared_ptr<Session>& s)
		: Consumer(stream, s), connection_(connection), send_flv_header_(false)
	{
#if 0
		// 测试使用的
		capture_type_ = true;
		StartCapture();
#endif //
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief FLV消费者析构函数（FLV Consumer Destructor）
	*  
	*  该析构函数用于清理FLV消费者实例，重置FLV头部发送标志。
	*  
	*  清理流程：
	*  1. 设置send_flv_header_标志为false
	*  2. HTTP连接对象由外部管理，不需要在此释放
	*  3. 基类Consumer的析构函数会自动调用，清理基类资源
	*  
	*  @note HTTP连接对象的生命周期由外部管理
	*  @note 流对象和会话对象由智能指针管理，会自动释放
	*/
	FlvConsumer::~FlvConsumer()
	{
		send_flv_header_ = false;
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理视频帧（On Video Frame）
	*  
	*  该方法用于处理从Stream接收到的视频帧，将其封装为FLV视频标签并通过HTTP连接发送。
	*  
	*  处理流程：
	*  1. 检查视频帧大小是否有效，如果为0则直接返回
	*  2. 从HTTP连接对象中获取FLV编码器上下文
	*  3. 如果上下文不存在，记录警告日志并返回
	*  4. 如果尚未发送FLV头部，则发送FLV头部（包含音频和视频标志）
	*  5. 将视频帧封装为FLV视频标签，设置时间戳（转换为毫秒）
	*  6. 通过HTTP连接发送FLV视频标签
	*  
	*  FLV视频标签格式：
	*  - 标签类型：0x09（视频标签）
	*  - 数据大小：视频帧大小 + 5字节（帧类型、编码ID、AVC包类型、组合时间戳）
	*  - 时间戳：视频帧的呈现时间戳（毫秒）
	*  - 流ID：通常为0
	*  - 数据内容：帧类型（1字节）+ 编码ID（1字节）+ AVC包类型（1字节）+ 组合时间戳（3字节）+ 视频数据
	*  
	*  时间戳转换：
	*  - 视频帧的时间戳单位通常为微秒（μs）
	*  - FLV标签的时间戳单位为毫秒（ms）
	*  - 转换公式：FLV时间戳 = 视频帧时间戳 / 1000
	*  
	*  @param frame 编码后的视频帧，包含H264编码数据和元信息
	*  @note 该方法会在Stream分发视频帧时被调用
	*  @note FLV头部只会发送一次，后续只发送FLV标签
	*  @note 时间戳会自动转换为毫秒单位
	*/
	void FlvConsumer::OnVideoFrame(const libmedia_codec::EncodedImage & frame)
	{
		if (frame.size() <= 0)
		{
			return;
		}
		auto context = connection_->GetContext<libmedia_transfer_protocol::libflv::FlvEncoder>(libmedia_transfer_protocol::libnetwork::kFlvContext);
		if (!context)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "flv consumer get flv context == null !!!";
			return;
		}
		if (!send_flv_header_)
		{
			send_flv_header_ = true;
			context->SendFlvHeader(true, true);
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "ts:" << rtc::TimeMillis() << ", f:" << frame.Timestamp();
		context->SendFlvVideoFrame(rtc::CopyOnWriteBuffer(frame.data(), frame.size()), frame.Timestamp() /1000 /*frame.Timestamp()*/ );


	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理音频帧（On Audio Frame）
	*  
	*  该方法用于处理从Stream接收到的音频帧，将其封装为FLV音频标签并通过HTTP连接发送。
	*  
	*  处理流程：
	*  1. 从HTTP连接对象中获取FLV编码器上下文
	*  2. 如果上下文不存在，记录警告日志并返回
	*  3. 如果尚未发送FLV头部，则发送FLV头部（包含音频和视频标志）
	*  4. 复制音频帧数据到新的缓冲区
	*  5. 将音频帧封装为FLV音频标签，设置时间戳（转换为毫秒）
	*  6. 通过HTTP连接发送FLV音频标签
	*  
	*  FLV音频标签格式：
	*  - 标签类型：0x08（音频标签）
	*  - 数据大小：音频帧大小 + 2字节（音频格式、AAC包类型）
	*  - 时间戳：音频帧的呈现时间戳（毫秒）
	*  - 流ID：通常为0
	*  - 数据内容：音频格式（1字节）+ AAC包类型（1字节）+ 音频数据
	*  
	*  音频格式字段（1字节）：
	*  - 高4位：音频编码格式（10 = AAC）
	*  - 第3-4位：采样率索引（0 = 5.5kHz, 1 = 11kHz, 2 = 22kHz, 3 = 44kHz）
	*  - 第2位：采样精度（0 = 8位, 1 = 16位）
	*  - 第1位：声道数（0 = 单声道, 1 = 立体声）
	*  
	*  时间戳转换：
	*  - 音频帧的时间戳单位通常为微秒（μs）
	*  - FLV标签的时间戳单位为毫秒（ms）
	*  - 转换公式：FLV时间戳 = 音频帧时间戳 / 1000
	*  
	*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据
	*  @param pts 呈现时间戳（Presentation Time Stamp），单位为微秒
	*  @note 该方法会在Stream分发音频帧时被调用
	*  @note FLV头部只会发送一次，后续只发送FLV标签
	*  @note 时间戳会自动转换为毫秒单位
	*  @note FLV的PTS精确到毫秒级别，微秒级别的精度会丢失
	*/
	void FlvConsumer::OnAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	{
		//return;
		auto context = connection_->GetContext<libmedia_transfer_protocol::libflv::FlvEncoder>(libmedia_transfer_protocol::libnetwork::kFlvContext);
		if (!context)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "flv consumer get flv context == null !!!";
			return;
		}
		if (!send_flv_header_)
		{
			send_flv_header_ = true;
			context->SendFlvHeader(true, true);
		}
		rtc::CopyOnWriteBuffer  new_frame = frame;
		// flv 的 pts精确到毫秒级别
		context->SendFlvAudioFrame(new_frame, pts / 1000);

	}
}
