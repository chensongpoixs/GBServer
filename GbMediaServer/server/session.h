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
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-14

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
#ifndef _C_GB_MEDIA_SERVER_SESSIOIN_H____
#define _C_GB_MEDIA_SERVER_SESSIOIN_H____


#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include "rtc_base/copy_on_write_buffer.h"
#include "producer/producer.h"
#include "consumer/consumer.h"
#include "server/stream.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
namespace  gb_media_server
{
	//class Consumer;
	//class Producer;
	//class Stream;
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		explicit Session(const std::string & session_name);
		virtual	~Session();
	public:

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 创建生产者（Create Producer）
		*  
		*  该方法用于在会话中创建一个生产者实例。生产者负责接收和推送媒体流到服务器。
		*  根据不同的协议类型（GB28181、RTC、RTSP等），创建相应类型的生产者。
		*  
		*  生产者类型说明：
		*  - kProducerTypeGB28181: 创建GB28181协议的生产者，用于接收GB28181设备的媒体流
		*  - kProducerTypeRtc: 创建RTC协议的生产者，用于接收WebRTC的媒体流
		*  - kProducerTypeRtsp: 创建RTSP协议的生产者，用于接收RTSP推流的媒体流
		*  
		*  创建流程：
		*  1. 验证会话名称是否匹配
		*  2. 解析会话名称（格式为 "app/stream"）
		*  3. 根据类型创建对应的生产者实例
		*  4. 设置应用名称和流名称
		*  5. 设置生产者的参数
		*  
		*  @param session_name 会话名称，格式为 "app/stream"，必须与当前会话名称匹配
		*  @param param 生产者的附加参数，用于传递协议特定的配置信息
		*  @param type 生产者类型，用于指定创建哪种协议的生产者
		*  @return 返回指向生产者对象的共享指针，如果创建失败则返回空指针
		*  @note 会话名称必须与当前会话名称完全匹配，否则创建失败
		*  
		*  使用示例：
		*  @code
		*  auto producer = session->CreateProducer("live/stream1", "", ShareResourceType::kProducerTypeRtc);
		*  if (producer) {
		*      session->SetProducer(producer);
		*  }
		*  @endcode
		*/
		std::shared_ptr<Producer> CreateProducer( 
			const std::string &session_name,
			const std::string &param,
			ShareResourceType type);
		std::shared_ptr<Consumer> CreateConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,   
			const std::string &session_name,
			const std::string &param,
			ShareResourceType type);
		 
		void AddConsumer(const std::shared_ptr<Consumer> & consumer);
		void RemoveConsumer(const std::shared_ptr<Consumer> & consumer);
		
		
		void ActiveAllPlayers(); 
		void SetProducer(std::shared_ptr<Producer>  producer);
		
		void  AddVideoFrame(  libmedia_codec::EncodedImage&&frame);
		 

		void  AddAudioFrame(  rtc::CopyOnWriteBuffer&& frame, int64_t pts);



		void AddDataChannel(const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, uint32_t ppid, const uint8_t* msg, size_t len);
		// ����ؼ�֡
		void ConsumerRequestKeyFrame();

	public:

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取流对象（Get Stream）
		*  
		*  该方法用于获取会话关联的流对象。流对象负责管理媒体流的存储和分发，
		*  包括HLS播放列表和分片文件的管理。
		*  
		*  @return 返回指向流对象的共享指针，如果流不存在则返回空指针
		*  @note 流对象在会话创建时自动创建，通常不会为空
		*  
		*  使用示例：
		*  @code
		*  auto stream = session->GetStream();
		*  if (stream) {
		*      std::string playlist = stream->GetPlayList();
		*  }
		*  @endcode
		*/
		std::shared_ptr<Stream> GetStream();

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取会话名称（Get Session Name）
		*  
		*  该方法用于获取会话的名称。会话名称通常采用 "app/stream" 格式，
		*  其中 app 表示应用名称，stream 表示流名称。
		*  
		*  会话名称说明：
		*  - 会话名称用于唯一标识一个媒体流会话
		*  - 格式通常为 "app/stream"，如 "live/stream1"
		*  - 在URL中使用，如 rtsp://server/live/stream1
		*  - 用于区分不同的应用和流
		*  
		*  @return 返回会话名称的常量引用
		*  @note 会话名称在会话创建时设置，通常不会更改
		*  
		*  使用示例：
		*  @code
		*  const std::string& session_name = session->SessionName();
		*  // session_name 可能是 "live/stream1"
		*  @endcode
		*/
		const std::string &SessionName()const;
		 
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 检查是否有生产者（Is Producer）
		*  
		*  该方法用于检查会话是否已经设置了生产者。如果会话有生产者，说明
		*  有客户端正在推送媒体流；如果没有生产者，说明该会话可能只是转发流。
		*  
		*  @return 如果有生产者返回 true，否则返回 false
		*  @note 该方法线程安全，不会阻塞
		*  
		*  使用示例：
		*  @code
		*  if (session->IsProducer()) {
		*      // 会话有生产者，是本地流
		*  } else {
		*      // 会话没有生产者，可能是转发流
		*  }
		*  @endcode
		*/
		bool IsProducer() const;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 清理会话资源（Clear）
		*  
		*  该方法用于清理会话的所有资源，包括生产者、消费者等。调用此方法后，
		*  会话将不再接收和分发媒体流，所有关联的资源将被释放。
		*  
		*  清理流程：
		*  1. 释放生产者对象
		*  2. 释放所有消费者对象
		*  3. 清空消费者集合
		*  
		*  @note 该方法线程安全，使用互斥锁保护
		*  @note 调用此方法后，会话将不再可用，建议先停止所有相关操作
		*  
		*  使用示例：
		*  @code
		*  session->Clear();
		*  @endcode
		*/
		void Clear();
	
	private: 
	private:
		std::string session_name_;
		 
		std::unordered_set<std::shared_ptr<Consumer>>    consumers_;
		std::shared_ptr<Stream>							 stream_{ nullptr };
		 
		 std::shared_ptr<Producer>								 producer_{ nullptr };
		std::mutex								lock_;
		std::atomic<int64_t>			     player_live_time_;

		//PullerRelay * pull_{ nullptr };
	};
}

#endif// 