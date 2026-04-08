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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 会话管理类（Session）
	 *  
	 *  该类是媒体服务器的核心会话管理类，负责管理一个媒体流会话的完整生命周期。
	 *  一个会话代表一个独立的媒体流，包含一个生产者和多个消费者。
	 *  
	 *  主要功能：
	 *  1. 生产者管理 - 创建和管理媒体流的生产者（推流端）
	 *  2. 消费者管理 - 创建和管理多个消费者（拉流端）
	 *  3. 流管理 - 管理媒体流对象，处理音视频数据
	 *  4. 数据分发 - 将生产者的音视频数据分发给所有消费者
	 *  5. 数据通道 - 支持WebRTC DataChannel数据传输
	 *  6. 关键帧请求 - 消费者可以请求生产者发送关键帧
	 *  
	 *  会话架构：
	 *  ```
	 *  Session
	 *    ├── Producer (1个) - 推流端
	 *    │     └── 接收音视频数据
	 *    ├── Stream (1个) - 流管理
	 *    │     ├── 缓存音视频帧
	 *    │     └── HLS切片管理
	 *    └── Consumers (多个) - 拉流端
	 *          └── 接收并发送音视频数据
	 *  ```
	 *  
	 *  数据流向：
	 *  Producer -> Session -> Stream -> Consumers
	 *  
	 *  支持的协议：
	 *  - GB28181: 国标视频监控协议
	 *  - WebRTC: 实时音视频通信协议
	 *  - RTSP: 实时流协议
	 *  - HTTP-FLV: 基于HTTP的FLV流媒体协议
	 *  - HLS: HTTP Live Streaming协议
	 *  
	 *  线程安全：
	 *  - 使用互斥锁保护消费者集合
	 *  - 音视频数据分发在工作线程中异步执行
	 *  
	 *  @note 该类使用enable_shared_from_this，支持安全的shared_ptr管理
	 *  @note 会话名称格式为 "app/stream"，如 "live/stream1"
	 *  @note 一个会话只能有一个生产者，但可以有多个消费者
	 */
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 构造函数
		 *  
		 *  初始化会话实例，创建流对象并设置会话名称。
		 *  
		 *  @param session_name 会话名称，格式为 "app/stream"
		 *  @note 使用explicit防止隐式类型转换
		 *  @note 会话名称在创建后不可更改
		 */
		explicit Session(const std::string & session_name);
		

		void CheckTimeOut();
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 析构函数
		 *  
		 *  清理会话资源，释放所有消费者。
		 *  
		 *  @note 析构时会自动清空消费者集合
		 *  @note 生产者和流对象由智能指针自动管理
		 */
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
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 创建消费者（Create Consumer）
		 *  
		 *  该方法用于在会话中创建一个消费者实例。消费者负责接收媒体流并发送给客户端。
		 *  根据不同的协议类型（RTC、FLV、RTSP等），创建相应类型的消费者。
		 *  
		 *  消费者类型说明：
		 *  - kConsumerTypeRTC: 创建WebRTC消费者，用于WebRTC拉流
		 *  - kConsumerTypeFlv: 创建HTTP-FLV消费者，用于HTTP-FLV拉流
		 *  - kConsumerTypeRtsp: 创建RTSP消费者，用于RTSP拉流
		 *  
		 *  创建流程：
		 *  1. 验证会话名称是否匹配
		 *  2. 解析会话名称（格式为 "app/stream"）
		 *  3. 根据类型创建对应的消费者实例
		 *  4. 设置应用名称和流名称
		 *  5. 设置消费者的参数
		 *  
		 *  @param conn 网络连接对象指针，用于与客户端通信
		 *  @param session_name 会话名称，格式为 "app/stream"，必须与当前会话名称匹配
		 *  @param param 消费者的附加参数，用于传递协议特定的配置信息
		 *  @param type 消费者类型，用于指定创建哪种协议的消费者
		 *  @return 返回指向消费者对象的共享指针，如果创建失败则返回空指针
		 *  @note 会话名称必须与当前会话名称完全匹配，否则创建失败
		 *  @note 会话名称必须包含至少两个部分（app和stream），用斜杠分隔
		 *  
		 *  使用示例：
		 *  @code
		 *  auto consumer = session->CreateConsumer(conn, "live/stream1", "", ShareResourceType::kConsumerTypeRTC);
		 *  if (consumer) {
		 *      session->AddConsumer(consumer);
		 *  }
		 *  @endcode
		 */
		std::shared_ptr<Consumer> CreateConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,   
			const std::string &session_name,
			const std::string &param,
			ShareResourceType type);
		 
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 添加消费者（Add Consumer）
		 *  
		 *  将消费者添加到会话的消费者集合中，使其能够接收媒体流。
		 *  
		 *  添加流程：
		 *  1. 使用互斥锁保护消费者集合
		 *  2. 将消费者插入到集合中
		 *  3. 记录日志，区分本地流和转发流
		 *  
		 *  @param consumer 要添加的消费者对象的共享指针
		 *  @note 该方法线程安全，使用互斥锁保护
		 *  @note 添加后，消费者会自动接收后续的音视频帧
		 *  @note 如果会话没有生产者，说明是转发流（relay）
		 */
		void AddConsumer(const std::shared_ptr<Consumer> & consumer);
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 移除消费者（Remove Consumer）
		 *  
		 *  从会话的消费者集合中移除指定的消费者，停止向其发送媒体流。
		 *  
		 *  移除流程：
		 *  1. 使用互斥锁保护消费者集合
		 *  2. 记录消费者信息（远程地址、引用计数等）
		 *  3. 从集合中移除消费者
		 *  4. 更新播放器活跃时间
		 *  
		 *  @param consumer 要移除的消费者对象的共享指针
		 *  @note 该方法线程安全，使用互斥锁保护
		 *  @note 移除后，消费者不再接收音视频帧
		 *  @note 通常在客户端断开连接时调用
		 */
		void RemoveConsumer(const std::shared_ptr<Consumer> & consumer);
		//void RemoveConsumer(const std::weak_ptr<Consumer>& consumer);
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 激活所有播放器（Active All Players）
		 *  
		 *  激活会话中的所有消费者，使其进入活跃状态。
		 *  
		 *  @note 该方法线程安全，使用互斥锁保护
		 *  @note 当前实现为空，功能已注释
		 */
		void ActiveAllPlayers(); 
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 设置生产者（Set Producer）
		 *  
		 *  设置会话的生产者，如果已有生产者则先释放旧的生产者。
		 *  
		 *  设置流程：
		 *  1. 使用互斥锁保护生产者对象
		 *  2. 检查是否与当前生产者相同
		 *  3. 如果已有生产者，先释放旧的生产者
		 *  4. 设置新的生产者
		 *  
		 *  @param producer 要设置的生产者对象的共享指针
		 *  @note 该方法线程安全，使用互斥锁保护
		 *  @note 一个会话只能有一个生产者
		 *  @note 设置新生产者会自动释放旧生产者
		 */
		void SetProducer(std::shared_ptr<Producer>  producer);
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 添加视频帧（Add Video Frame）
		 *  
		 *  接收来自生产者的视频帧，并异步分发给所有消费者。
		 *  
		 *  处理流程：
		 *  1. 接收编码后的视频帧（使用移动语义）
		 *  2. 将分发任务投递到工作线程
		 *  3. 在工作线程中遍历所有消费者
		 *  4. 将视频帧发送给每个消费者
		 *  
		 *  @param frame 编码后的视频帧，使用移动语义避免拷贝
		 *  @note 该方法可能在生产者线程中调用
		 *  @note 实际分发在工作线程中异步执行，避免阻塞生产者
		 *  @note 使用std::move传递帧数据，调用后frame将失效
		 *  @note 支持调试模式，可以将视频帧保存到文件（通过宏控制）
		 */
		void  AddVideoFrame(  libmedia_codec::EncodedImage&&frame);
		 
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 添加音频帧（Add Audio Frame）
		 *  
		 *  接收来自生产者的音频帧，并异步分发给所有消费者。
		 *  
		 *  处理流程：
		 *  1. 接收编码后的音频帧和时间戳（使用移动语义）
		 *  2. 将分发任务投递到工作线程
		 *  3. 在工作线程中遍历所有消费者
		 *  4. 将音频帧发送给每个消费者
		 *  
		 *  @param frame 编码后的音频帧数据，使用移动语义避免拷贝
		 *  @param pts 音频帧的显示时间戳（Presentation Timestamp）
		 *  @note 该方法可能在生产者线程中调用
		 *  @note 实际分发在工作线程中异步执行，避免阻塞生产者
		 *  @note 使用std::move传递帧数据，调用后frame将失效
		 */
		void  AddAudioFrame(  rtc::CopyOnWriteBuffer&& frame, int64_t pts);

		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 添加数据通道消息（Add Data Channel）
		 *  
		 *  接收WebRTC DataChannel的数据消息，并异步分发给所有消费者和生产者。
		 *  
		 *  处理流程：
		 *  1. 接收SCTP流参数、PPID和消息数据
		 *  2. 将消息数据拷贝到Buffer中
		 *  3. 将分发任务投递到工作线程
		 *  4. 在工作线程中分发给所有消费者
		 *  5. 如果有生产者，也分发给生产者
		 *  
		 *  DataChannel用途：
		 *  - 传输文本消息
		 *  - 传输二进制数据
		 *  - 传输控制信令
		 *  
		 *  @param params SCTP流参数，包含流ID等信息
		 *  @param ppid Payload Protocol Identifier，标识数据类型
		 *  @param msg 消息数据指针
		 *  @param len 消息数据长度
		 *  @note 该方法可能在WebRTC线程中调用
		 *  @note 实际分发在工作线程中异步执行
		 *  @note 消息数据会被拷贝，原始数据可以安全释放
		 */
		void AddDataChannel(const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, uint32_t ppid, const uint8_t* msg, size_t len);
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 消费者请求关键帧（Consumer Request Key Frame）
		 *  
		 *  当消费者需要关键帧时（如新消费者加入或丢包严重），向生产者请求发送关键帧。
		 *  
		 *  处理流程：
		 *  1. 检查是否有生产者
		 *  2. 如果没有生产者，直接返回
		 *  3. 将请求任务投递到工作线程
		 *  4. 在工作线程中调用生产者的RequestKeyFrame方法
		 *  
		 *  关键帧作用：
		 *  - 新消费者加入时需要关键帧才能开始解码
		 *  - 网络丢包严重时需要关键帧恢复画面
		 *  - 关键帧包含完整的图像信息，可独立解码
		 *  
		 *  @note 该方法可能在消费者线程中调用
		 *  @note 实际请求在工作线程中异步执行
		 *  @note 如果没有生产者，请求会被忽略
		 */
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
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 会话名称（Session Name）
		 *  
		 *  该会话的唯一标识符，格式为 "app/stream"。
		 *  
		 *  命名规则：
		 *  - 格式：app/stream
		 *  - 示例：live/stream1, vod/movie1
		 *  - app: 应用名称，用于区分不同的应用场景
		 *  - stream: 流名称，用于区分同一应用下的不同流
		 *  
		 *  @note 会话名称在构造时设置，之后不可更改
		 *  @note 用于日志记录、流查找和管理
		 */
		std::string session_name_;

		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 消费者集合（Consumers）
		 *  
		 *  存储该会话的所有消费者对象。
		 *  
		 *  集合特点：
		 *  - 使用unordered_set存储，查找效率高
		 *  - 使用shared_ptr管理消费者生命周期
		 *  - 支持多个消费者同时拉流
		 *  - 使用互斥锁保护，确保线程安全
		 *  
		 *  @note 消费者数量没有限制，取决于服务器性能
		 *  @note 添加和移除操作都需要加锁
		 */
		std::unordered_set<std::shared_ptr<Consumer>>    consumers_;
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 流对象（Stream）
		 *  
		 *  管理该会话的媒体流，负责音视频数据的缓存和HLS切片管理。
		 *  
		 *  流对象功能：
		 *  - 缓存音视频帧
		 *  - 管理GOP（Group of Pictures）
		 *  - 生成HLS播放列表和TS切片
		 *  - 提供流信息查询
		 *  
		 *  @note 流对象在会话构造时创建
		 *  @note 使用shared_ptr管理生命周期
		 *  @note 初始化为nullptr，在构造函数中创建
		 */
		std::shared_ptr<Stream>							 stream_{ nullptr };
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 生产者对象（Producer）
		 *  
		 *  该会话的媒体流生产者，负责接收和推送媒体流。
		 *  
		 *  生产者类型：
		 *  - Gb28181Producer: GB28181协议生产者
		 *  - RtcProducer: WebRTC协议生产者
		 *  - RtspProducer: RTSP协议生产者
		 *  
		 *  @note 一个会话只能有一个生产者
		 *  @note 使用shared_ptr管理生命周期
		 *  @note 初始化为nullptr，通过SetProducer设置
		 *  @note 如果为nullptr，说明该会话可能是转发流
		 */
		 std::shared_ptr<Producer>								 producer_{ nullptr };
		 
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 互斥锁（Lock）
		 *  
		 *  保护消费者集合和生产者对象的互斥锁。
		 *  
		 *  保护的操作：
		 *  - 添加消费者
		 *  - 移除消费者
		 *  - 设置生产者
		 *  - 清理会话资源
		 *  
		 *  @note 使用std::lock_guard自动管理锁的生命周期
		 *  @note 避免在持有锁时执行耗时操作
		 */
		std::mutex								lock_;
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 播放器活跃时间（Player Live Time）
		 *  
		 *  记录播放器最后活跃的时间戳，用于检测会话超时。
		 *  
		 *  用途：
		 *  - 记录最后一次消费者操作的时间
		 *  - 用于检测会话是否长时间无活动
		 *  - 用于会话超时清理
		 *  
		 *  @note 使用atomic保证线程安全
		 *  @note 时间戳单位为毫秒
		 *  @note 在构造函数和RemoveConsumer中更新
		 */
		std::atomic<int64_t>			     player_live_time_;
		// 定时器相关
		 rtc::scoped_refptr<webrtc::PendingTaskSafetyFlag> rtc_task_safety_;
		//PullerRelay * pull_{ nullptr };
	};
}

#endif// 