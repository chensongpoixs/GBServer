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

 purpose:		RtmpConsumer
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
#include "consumer/crtmp_consumer.h" 
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "gb_media_server_log.h"
namespace gb_media_server
{
	/**
	*  @brief 构造RTMP消费者实例
	*  
	*  初始化RTMP消费者，保存网络连接指针和流对象。
	*  RTMP消费者负责将媒体流封装为RTMP格式并发送给客户端。
	*/
	RtmpConsumer::RtmpConsumer(libmedia_transfer_protocol::libnetwork::Connection*    connection,
		std::shared_ptr<Stream>& stream,const std::shared_ptr<Session>& s)
		: Consumer(stream, s), connection_(connection) 
	{
#if 0
		// 测试使用的
		capture_type_ = true;
		StartCapture();
#endif //
	}
	
	/**
	*  @brief 析构RTMP消费者实例
	*  
	*  清理RTMP消费者相关资源。网络连接由外部管理，不需要在此释放。
	*/
	RtmpConsumer::~RtmpConsumer()
	{
		 
	}
	
	/**
	*  @brief 处理视频帧
	*  
	*  将视频帧封装为RTMP视频消息并发送给客户端。
	*  该方法会检查视频帧的有效性，并从网络连接上下文中获取RTMP编码器。
	*  
	*  处理流程：
	*  1. 检查视频帧大小是否有效
	*  2. 获取RTMP编码器上下文（当前代码中被注释掉）
	*  3. 封装视频帧为RTMP消息并发送
	*  
	*  @note 当前实现被注释掉，需要根据实际需求实现RTMP视频封装逻辑
	*/
	void RtmpConsumer::OnVideoFrame(const libmedia_codec::EncodedImage & frame)
	{
		if (frame.size() <= 0)
		{
			return;
		}
		//auto context = connection_->GetContext<libmedia_transfer_protocol::libflv::FlvEncoder>(libmedia_transfer_protocol::libnetwork::kFlvContext);
		//if (!context)
		//{
		//	GBMEDIASERVER_LOG_T_F(LS_WARNING) << "flv consumer get flv context == null !!!";
		//	return;
		//}
		//if (!send_flv_header_)
		//{
		//	send_flv_header_ = true;
		//	context->SendFlvHeader(true, true);
		//}
		////GBMEDIASERVER_LOG(LS_INFO) << "ts:" << rtc::TimeMillis() << ", f:" << frame.Timestamp();
		//context->SendFlvVideoFrame(rtc::CopyOnWriteBuffer(frame.data(), frame.size()), frame.Timestamp() /1000 /*frame.Timestamp()*/ );


	}
	
	/**
	*  @brief 处理音频帧
	*  
	*  将音频帧封装为RTMP音频消息并发送给客户端。
	*  该方法会从网络连接上下文中获取RTMP编码器，并将音频帧封装为RTMP消息。
	*  
	*  处理流程：
	*  1. 获取RTMP编码器上下文（当前代码中被注释掉）
	*  2. 封装音频帧为RTMP消息并发送
	*  3. 时间戳转换为毫秒级别
	*  
	*  @note 当前实现被注释掉，需要根据实际需求实现RTMP音频封装逻辑
	*  @note RTMP的PTS精度为毫秒级别，需要从微秒转换（除以1000）
	*/
	void RtmpConsumer::OnAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	{
		//return;
		//auto context = connection_->GetContext<libmedia_transfer_protocol::libflv::FlvEncoder>(libmedia_transfer_protocol::libnetwork::kFlvContext);
		//if (!context)
		//{
		//	GBMEDIASERVER_LOG_T_F(LS_WARNING) << "flv consumer get flv context == null !!!";
		//	return;
		//}
		//if (!send_flv_header_)
		//{
		//	send_flv_header_ = true;
		//	context->SendFlvHeader(true, true);
		//}
		//rtc::CopyOnWriteBuffer  new_frame = frame;
		//// flv 的 pts精确到毫秒级别
		//context->SendFlvAudioFrame(new_frame, pts / 1000);

	}
}
