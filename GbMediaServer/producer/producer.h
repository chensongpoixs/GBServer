

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
 *
 *
 *
 *				   Author: chensong
 *				   date:  2025-04-26
 *
 * 输赢不重要，答案对你们有什么意义才重要。
 *
 * 光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。
 *
 *
 *我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
 *然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
 *3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
 *然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
 *于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
 *我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
 *从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
 *我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
 *沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
 *安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
 *
 *******************************************************************************/

#ifndef _C_GB_MEDIA_SERVER_PRODUCER_H_
#define _C_GB_MEDIA_SERVER_PRODUCER_H_



#include <cstdint>
#include <memory>   
#include <vector>  
#include <string>

#include <atomic>

#include "rtc_base/copy_on_write_buffer.h"
#include "rtc_base/socket_address.h"
#include "libmedia_codec/encoded_image.h"
//#include "server/session.h"
#include "share/share_resource.h"
namespace gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-04-26
	*  @brief 生产者基类（Producer Base Class）
	*  
	*  Producer是GBMediaServer流媒体服务器中所有生产者的抽象基类。
	*  生产者负责从外部源（如RTC客户端、RTSP推流、RTMP推流等）接收媒体数据，
	*  并将这些数据推送到Stream中，供消费者（Consumer）使用。
	*  
	*  生产者功能：
	*  1. 接收来自不同协议的媒体数据（RTC、RTSP、RTMP、GB28181等）
	*  2. 解析和处理协议特定的数据格式
	*  3. 将媒体数据推送到Stream中
	*  4. 处理关键帧请求（RequestKeyFrame）
	*  5. 管理流状态（SetStreamStatus）
	*  
	*  生产者类型：
	*  - RtcProducer: 处理WebRTC推流（DTLS、SRTP、RTP等）
	*  - RtspProducer: 处理RTSP推流（RTP over TCP/UDP）
	*  - RtmpProducer: 处理RTMP推流（FLV格式）
	*  - Gb28181Producer: 处理GB28181推流（国标协议）
	*  
	*  工作流程：
	*  1. 生产者接收来自外部源的数据包
	*  2. 解析数据包，提取媒体数据（音频/视频）
	*  3. 将媒体数据推送到Stream中
	*  4. Stream将数据分发给所有消费者
	*  
	*  @note Producer继承自ShareResource，可以与Stream和Session关联
	*  @note Producer是抽象基类，具体功能由子类实现
	*  @note 每个Stream可以有一个Producer和多个Consumer
	*  
	*  使用示例：
	*  @code
	*  // 创建RTC生产者
	*  auto producer = std::make_shared<RtcProducer>(stream, session);
	*  
	*  // 接收数据
	*  producer->OnRecv(buffer);
	*  
	*  // 请求关键帧
	*  producer->RequestKeyFrame();
	*  @endcode
	*/
		class Producer : public ShareResource
		{
		public:
			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 构造函数（Constructor）
			*  
			*  该构造函数用于初始化Producer实例。它会将生产者与Stream和Session关联，
			*  并初始化内部数据结构。
			*  
			*  初始化流程：
			*  1. 调用ShareResource基类构造函数，关联Stream和Session
			*  2. 初始化内部数据结构（缓冲区、解析器等）
			*  
			*  @param stream 指向Stream的共享指针，用于推送媒体数据
			*  @param s 指向Session的共享指针，用于管理会话状态
			*  @note 构造函数会自动将生产者注册到Stream中
			*  @note 每个Stream只能有一个Producer
			*/
			Producer(  const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
			
			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 析构函数（Destructor）
			*  
			*  该析构函数用于清理Producer实例。它会释放所有相关资源，
			*  并从Stream中注销生产者。
			*  
			*  清理流程：
			*  1. 停止接收数据
			*  2. 释放内部资源（缓冲区、解析器等）
			*  3. 从Stream中注销生产者
			*  
			*  @note 析构函数会自动调用，不需要手动释放资源
			*/
			virtual ~Producer() = default;
		public:

			 

			//���ܲ�ͬЭ���ϲ㴦�� ʵ��
			//virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}

			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 请求关键帧（Request Key Frame）
			*  
			*  该方法用于请求生产者发送一个关键帧（IDR帧）。当消费者需要快速恢复视频播放时，
			*  可以通过调用此方法请求一个关键帧，以便快速重建视频解码器状态。
			*  
			*  关键帧是视频编码中的重要概念：
			*  - 关键帧（I帧/IDR帧）是独立编码的帧，不需要参考其他帧即可解码
			*  - 非关键帧（P帧/B帧）需要参考其他帧才能解码
			*  - 当消费者刚开始播放或需要快速跳转时，需要从关键帧开始解码
			*  
			*  @note 该方法是虚函数，由具体的生产者实现类（如RtcProducer、RtspProducer等）重写
			*        来实现特定的关键帧请求逻辑。不同协议的关键帧请求机制可能不同。
			*  
			*  使用场景：
			*  - 新消费者加入时，需要快速获取关键帧以开始播放
			*  - 网络丢包导致解码失败时，需要请求关键帧恢复
			*  - 视频质量切换时，需要请求关键帧重新编码
			*  
			*  @note 对于某些协议（如RTC），请求关键帧是异步的，可能需要等待一段时间才能收到
			*/
			 virtual void RequestKeyFrame() {}
		


			/**
			*  stream    ��û���˹ۿ�ʱֹͣ����  ����������ʱ���Զ�������
			* @param bool: �Ƿ�������� 
			* return ����ֵ
			*/
			 virtual  void SetStreamStatus(bool status) {}
		public: 
		protected:
			 
		};
 
}

#endif // 