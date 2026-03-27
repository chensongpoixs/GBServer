
/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR


实时流

1. 一路实时输入的艺术品数据流
2. 服务器被动接收推流
3. 服务器自动拉流




- 处理输入音视频数据
	1. 修正时间戳， 处理index
	2. 处理CodecHeader
	3. 处理GOP
	4. 保存音视频帧

- 输出音视频数据



1. 定位GOP， 查找CodecHeader
2. 必须先发送CodecHeader， 再发送音视频数据
3. 跳帧，重新查找CodecHeader
4. 输出一定数量的音视频帧






										   流程图


			  请求音视频帧



			已经输出音视频序列头                     否定 ===>          |          定位GOP
																	|
																	|
				  是												    |
																	|
			音视频帧落后太多?                       是   ===>          |          定位GOP

				  否定                                                           ||

																			查找音视频序列头

																				 ||

																			输出音视频序列头


			输出一定数量的音视频帧




流超时


- 长时间没有收到实时数据， 记录收到数据的时间 当前时间收到数据的时间差， 超过一定的时间， 流就算超时
- 流超时是被动检测




流准备好

- 收到关键帧


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
#include "server/stream.h"
#include "server/session.h"
#include "rtc_base/time_utils.h"
 
namespace gb_media_server
{
	 
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 构造函数（Constructor）
		 *  
		 *  初始化Stream实例，设置会话引用和会话名称，创建HLS Muxer。
		 *  
		 *  初始化流程：
		 *  1. 保存会话引用
		 *  2. 保存会话名称
		 *  3. 创建HLS Muxer实例，使用会话名称作为标识
		 *  
		 *  @param s 所属的会话引用，用于访问会话功能和分发数据
		 *  @param session_name 会话名称，用于唯一标识该流
		 *  @note 会话对象的生命周期必须长于Stream对象
		 *  @note HLS Muxer在构造时创建，准备接收音视频数据
		 */
		Stream::Stream( Session &s, const std::string & session_name)
			:session_(s), session_name_(session_name) , hls_muxer_(session_name)
		{
			 
		}
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 添加视频帧（Add Video Frame）
		 *  
		 *  接收并处理来自生产者的编码后的视频帧，将其分发给所有消费者。
		 *  
		 *  处理流程：
		 *  1. 接收编码后的视频帧（使用移动语义）
		 *  2. TODO: 将视频帧传递给HLS Muxer进行TS封装
		 *  3. 将视频帧转发给会话，由会话分发给所有消费者
		 *  
		 *  视频帧类型：
		 *  - 关键帧（IDR帧）：包含完整的图像信息，可独立解码
		 *  - 非关键帧（P帧/B帧）：依赖其他帧解码
		 *  
		 *  @param frame 编码后的视频帧，包含编码数据、时间戳、帧类型等信息
		 *  @note 使用std::move避免数据拷贝，提高性能
		 *  @note 该方法会在生产者线程中调用
		 *  @note TODO: 需要实现HLS协议的视频帧处理
		 *  @note 视频帧会被转发到会话，由会话分发给所有消费者
		 */
		void Stream::AddVideoFrame(  libmedia_codec::EncodedImage&& frame)
		{ 
			// TODO@chensong 2025-11-17 实现 HLS的协议
			//hls_muxer_.OnPacket(packet);
			session_.AddVideoFrame(std::move(frame));
		}
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 添加音频帧（Add Audio Frame）
		 *  
		 *  接收并处理来自生产者的编码后的音频帧，将其分发给所有消费者。
		 *  
		 *  处理流程：
		 *  1. 接收编码后的音频帧和时间戳（使用移动语义）
		 *  2. TODO: 将音频帧传递给HLS Muxer进行TS封装
		 *  3. 将音频帧转发给会话，由会话分发给所有消费者
		 *  
		 *  音频帧特点：
		 *  - 音频帧通常较小（几百字节到几KB）
		 *  - 音频帧频率较高（如AAC每帧约23ms）
		 *  - 音频没有关键帧概念，每帧都可独立解码
		 *  - 音频时间戳用于音视频同步
		 *  
		 *  @param frame 编码后的音频帧数据（AAC/Opus等格式）
		 *  @param pts 音频帧的显示时间戳（Presentation Timestamp），单位通常为毫秒
		 *  @note 使用std::move避免数据拷贝，提高性能
		 *  @note 该方法会在生产者线程中调用
		 *  @note TODO: 需要实现HLS协议的音频帧处理
		 *  @note 音频帧会被转发到会话，由会话分发给所有消费者
		 */
		void Stream::AddAudioFrame(  rtc::CopyOnWriteBuffer&&frame, int64_t  pts)
		{
			// TODO@chensong 2025-11-17 实现 HLS的协议
			//hls_muxer_.OnPacket(packet);
			session_.AddAudioFrame(std::move(frame), pts);
		}
		 
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 获取会话名称（Session Name）
		 *  
		 *  返回该流所属的会话名称。
		 *  
		 *  @return const std::string& 会话名称的常量引用
		 *  @note 返回引用避免字符串拷贝，提高性能
		 *  @note 会话名称在Stream创建时设置，之后不可更改
		 */
		const std::string & Stream::SessionName() const
		{
			return session_name_;
		}
		 
	 
}

