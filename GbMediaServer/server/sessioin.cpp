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

#include "server/session.h" 
#include "rtc_base/time_utils.h"
#include "rtc_base/string_utils.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_encode.h"
#include <string>
#include "absl/strings/string_view.h"
#include "server/stream.h" 
#include "server/stream.h"
//#include "user/play_rtc_user.h"
#include <memory> 
#include "api/array_view.h"
#include "absl/strings/string_view.h"
#include "utils/string_utils.h" 
#include "consumer/rtc_consumer.h"
#include "producer/gb28181_producer.h"
#include "consumer/flv_consumer.h"
#include "server/gb_media_service.h"
#include "producer/rtc_producer.h"
#include "producer/crtsp_producer.h"
#include "consumer/crtsp_consumer.h"
#include "gb_media_server_log.h"

#include "server/rtc_service.h"
#include "server/rtc_service_mgr.h"



namespace  gb_media_server
{
	namespace
	{
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 空消费者对象（Null Consumer）
		 *  
		 *  用于在创建消费者失败时返回的空对象。
		 *  
		 *  @note 使用静态对象避免重复创建
		 *  @note 调用者应检查返回值是否为空
		 */
		static std::shared_ptr<Consumer> consumer_null;
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 空生产者对象（Null Producer）
		 *  
		 *  用于在创建生产者失败时返回的空对象。
		 *  
		 *  @note 使用静态对象避免重复创建
		 *  @note 调用者应检查返回值是否为空
		 */
		static std::shared_ptr<Producer> producer_null;
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 构造函数（Constructor）
	 *  
	 *  初始化会话实例，创建流对象并设置初始状态。
	 *  
	 *  初始化流程：
	 *  1. 初始化消费者集合为空
	 *  2. 保存会话名称
	 *  3. 创建Stream对象，传入会话引用和会话名称
	 *  4. 设置播放器活跃时间为当前时间
	 *  
	 *  @param session_name 会话名称，格式为 "app/stream"
	 *  @note Stream对象在构造时创建，确保会话始终有流对象
	 *  @note 播放器活跃时间用于检测会话超时
	 */
	Session::Session(const std::string & session_name, uint64_t  rtc_service_index)
		:consumers_()
		, session_name_(session_name)
		, rtc_task_safety_(webrtc::PendingTaskSafetyFlag::Create())
		, rtc_service_index_(rtc_service_index)
		, rtc_service_(RtcServiceMgr::GetInstance().GetRtcService(rtc_service_index_))
	{
		stream_ = std::make_shared<Stream>( *this, session_name);
		player_live_time_ = rtc::TimeMillis();
		CheckTimeOut();
	}

	void Session::CheckTimeOut()
	{
		GbMediaService::GetInstance().worker_thread()->PostDelayedTask(ToQueuedTask(rtc_task_safety_,
			[this]() {
				 

				if (producer_ && producer_->ShareResouceType() == kProducerTypeRtc && producer_->CheckStunTimeOut())
				{
					// 获取会话名称
					std::string session_name =  SessionName();

					// 在工作线程中执行清理操作
					//GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
						// 从RTC服务中注销RTC接口
						std::shared_ptr<RtcProducer> slef = std::dynamic_pointer_cast<RtcProducer>(producer_);
						rtc_service_->UnregisterRtcInterface(slef);// shared_from_this());

						// 清空Session的Producer
						 SetProducer(nullptr);
						//});
				}
				std::vector< std::shared_ptr<Consumer>> ccc;
				for (auto c: consumers_)
				{
					if (c && c->ShareResouceType() == kConsumerTypeRTC && c->CheckStunTimeOut())
					{
						ccc.push_back(c);
					}
					else if (c && c->ShareResouceType() == kConsumerTypeRTC)
					{
						std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(c);
						slef->OnTimer();
					}
				}
				for (size_t i = 0; i < ccc.size(); ++i)
				{
					std::string session_name =  SessionName();
					//GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
						std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(ccc[i]);
						rtc_service_->UnregisterRtcInterface(slef);
						//GbMediaService::GetInstance().CloseSession(session_name);
						 RemoveConsumer(slef);

					//	});
				}
				//if (rtc::TimeMillis() - rtc_stun_timestamp_ > (YamlConfig::GetInstance().GetRtcServerConfig().timeout_ms * 1)  && !destoy_)
				//{

				//	// 删除当前rtc 
				//	GBMEDIASERVER_LOG(LS_WARNING) << " rtc time out !!! diff ms :"<< (rtc::TimeMillis() - rtc_stun_timestamp_) << ",   config ms : " << YamlConfig::GetInstance().GetRtcServerConfig().timeout_ms;
				//	//if (!GetDestory())
				//	{

				//		RemoveGlobalData();
				//		destoy_ = true;
				//	}
				//	//GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << session_name;
				//		//http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				//		//	auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				//		//	http_ctx->PostRequest(res);
				//		//	http_ctx->WriteComplete(conn);
				//		//	});

				//	//auto s = GbMediaService::GetInstance().CreateSession(session_name);
				//	//if (!s)
				//	//{
				//	//	GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << session_name;
				//	//	//http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				//	//	//	auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				//	//	//	http_ctx->PostRequest(res);
				//	//	//	http_ctx->WriteComplete(conn);
				//	//	//	});

				//	//	return;
				//	//}
				//	return;

				//}
				//if (destoy_)
				//{
				//	return;
				//}

				// 递归调用实现定时循环（每5秒）
				CheckTimeOut();
			}), 5000);
	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 析构函数（Destructor）
	 *  
	 *  清理会话资源，释放所有消费者。
	 *  
	 *  清理流程：
	 *  1. 清空消费者集合
	 *  2. 生产者和流对象由智能指针自动释放
	 *  
	 *  @note 消费者集合的clear会减少引用计数
	 *  @note 如果消费者没有其他引用，会被自动销毁
	 */
	Session::~Session()
	{
		//if (pull_)
		//{
		//	delete pull_;
		//}
		consumers_.clear();
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 创建生产者（Create Producer）
	 *  
	 *  根据指定的协议类型创建相应的生产者实例。
	 *  
	 *  创建流程：
	 *  1. 验证会话名称是否匹配
	 *  2. 解析会话名称，分割为app和stream
	 *  3. 根据类型创建对应的生产者：
	 *     - GB28181: 创建Gb28181Producer
	 *     - RTC: 创建RtcProducer
	 *     - RTSP: 创建RtspProducer
	 *  4. 设置生产者的应用名称和流名称
	 *  5. 设置生产者的附加参数
	 *  
	 *  @param session_name 会话名称，必须与当前会话名称匹配
	 *  @param param 生产者的附加参数
	 *  @param type 生产者类型
	 *  @return 返回生产者对象的共享指针，失败返回空指针
	 *  @note 如果会话名称不匹配，记录错误日志并返回空指针
	 *  @note 如果类型不支持，返回空指针
	 */
	std::shared_ptr<Producer> Session::CreateProducer( 
		const std::string &session_name,
		const std::string &param,
		ShareResourceType type)
	{
		if (session_name != session_name_)
		{
			 
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish Producer failed !!! invalid session name: " << session_name;
			return producer_null;
		} 
		std::vector<std::string> list;
		string_utils::split(session_name, '/', & list);
		std::shared_ptr<Producer> producer;
		switch (type)
		{
			case ShareResourceType::kProducerTypeGB28181:
			{
				producer = std::make_shared<Gb28181Producer>( stream_, shared_from_this());
				break;
			}
			case ShareResourceType::kProducerTypeRtc:
			{
				producer = std::make_shared<RtcProducer>(stream_, shared_from_this());
				break;
			}
			case ShareResourceType::kProducerTypeRtsp:
			{
				producer = std::make_shared<RtspProducer>(stream_, shared_from_this());
				break;
			}
			 
			default:
			{
				return producer_null; 
				break;
			}
		}
		 
		producer->SetAppName(list[0]);
		producer->SetStreamName(list[1]);
		producer->SetParam(param);
		 
		 
		return producer;
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 创建消费者（Create Consumer）
	 *  
	 *  根据指定的协议类型创建相应的消费者实例。
	 *  
	 *  创建流程：
	 *  1. 验证会话名称是否匹配
	 *  2. 解析会话名称，分割为app和stream
	 *  3. 验证会话名称至少包含两个部分
	 *  4. 根据类型创建对应的消费者：
	 *     - RTC: 创建RtcConsumer
	 *     - FLV: 创建FlvConsumer
	 *     - RTSP: 创建RtspConsumer
	 *  5. 设置消费者的应用名称和流名称
	 *  6. 设置消费者的附加参数
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param session_name 会话名称，必须与当前会话名称匹配
	 *  @param param 消费者的附加参数
	 *  @param type 消费者类型
	 *  @return 返回消费者对象的共享指针，失败返回空指针
	 *  @note 如果会话名称不匹配，记录错误日志并返回空指针
	 *  @note 如果会话名称格式不正确（少于2个部分），返回空指针
	 *  @note 如果类型不支持，返回空指针
	 */
	std::shared_ptr<Consumer> Session::CreateConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,
		const std::string &session_name,
		const std::string &param,
		ShareResourceType type)
	{
		if (session_name != session_name_)
		{
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish Consumer failed.Invalid session name:" << session_name;
			return consumer_null;
		} 
		std::vector<std::string> list; 
		string_utils:: split(/*rtc::ArrayView*/( session_name ), '/', &list);
		if (list.size() < 2)
		{
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish user failed.Invalid session name:" << session_name;
			return consumer_null;
		}
		std::shared_ptr< Consumer> consumer;
		if (type == ShareResourceType::kConsumerTypeRTC)
		{
			consumer = std::make_shared<RtcConsumer>(  stream_, shared_from_this());
		} 
		else if (type == ShareResourceType::kConsumerTypeFlv)
		{
			consumer = std::make_shared<FlvConsumer>(conn, stream_, shared_from_this());
		}
		else if (type == ShareResourceType::kConsumerTypeRtsp)
		{
			consumer = std::make_shared<RtspConsumer>(conn, stream_, shared_from_this());
		}
		else
		{
			return consumer_null;
		} 
		consumer->SetAppName(list[0]);
		consumer->SetStreamName(list[1]);
		consumer->SetParam(param);
		 
		return consumer;
	}
	 
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 激活所有播放器（Active All Players）
	 *  
	 *  激活会话中的所有消费者。
	 *  
	 *  @note 该方法线程安全，使用互斥锁保护
	 *  @note 当前实现为空，功能已注释
	 */
	void Session::ActiveAllPlayers()
	{
		std::lock_guard<std::mutex> lk(lock_);
		//for (auto const &u : consumers_)
		//{
		//	u->Active();
		//}
	}
	void Session::AddConsumer(const std::shared_ptr< Consumer> &consumer)
	{
		{
			std::lock_guard<std::mutex> lk(lock_);
			consumers_.insert(consumer);
		}



		if (!producer_)
		{
			GBMEDIASERVER_LOG(INFO) << " add consumer,  realy  -->  session name:" << session_name_ << ",consumer id:" << consumer->RemoteAddress().ToString();

		 
		}
		else
		{
			GBMEDIASERVER_LOG(INFO) << " add consumer,  local stream   -->  session name:" << session_name_ << ",consumer id:" << consumer->RemoteAddress().ToString();

		} 
	}
	 
	void Session::RemoveConsumer(const std::shared_ptr<Consumer> & consumer)
	{ 
				
				
		{
			std::lock_guard<std::mutex> lk(lock_);
	// ���ʹ������ͷŶ�������� �޸�bug 
			GBMEDIASERVER_LOG(INFO) << "remove consumer,session name:" << session_name_
				<< ",remoteaddr:" << consumer->RemoteAddress().ToString()
				//<< ",elapsed:" << consumer->ElapsedTime()
				//<< ",ReadyTime:" << ReadyTime()
				//<< ",stream time:" << SinceStart() 
				<< ", use_count: " << consumer.use_count();
			consumers_.erase(consumer);
			player_live_time_ = rtc::TimeMillis();
		}
		  
		 
	}
	//void Session::RemoveConsumer(const std::weak_ptr<Consumer>& consumer)
	//{
	//	auto ptr = consumer.lock();
	//	if (!ptr) return;  // 对象已销毁，直接返回
	//	{
	//		std::lock_guard<std::mutex> lk(lock_);
	//		// ���ʹ������ͷŶ�������� �޸�bug 
	//		GBMEDIASERVER_LOG(INFO) << "remove consumer,session name:" << session_name_
	//			<< ",remoteaddr:" << ptr->RemoteAddress().ToString()
	//			//<< ",elapsed:" << consumer->ElapsedTime()
	//			//<< ",ReadyTime:" << ReadyTime()
	//			//<< ",stream time:" << SinceStart() 
	//			<< ", use_count: " << consumer.use_count();
	//		consumers_.erase(ptr);
	//		player_live_time_ = rtc::TimeMillis();
	//	}

	//}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 设置生产者（Set Producer）
	 *  
	 *  设置会话的生产者，如果已有生产者则先释放旧的生产者。
	 *  
	 *  设置流程：
	 *  1. 使用互斥锁保护生产者对象
	 *  2. 检查新生产者是否与当前生产者相同
	 *  3. 如果已有生产者，先重置（释放）旧的生产者
	 *  4. 如果新生产者不为空，设置新的生产者
	 *  
	 *  @param producer 要设置的生产者对象的共享指针
	 *  @note 该方法线程安全，使用互斥锁保护
	 *  @note 如果新旧生产者相同，会记录警告日志
	 *  @note 一个会话只能有一个生产者
	 */
	void Session::SetProducer(std::shared_ptr<Producer>  producer)
	{
		std::lock_guard<std::mutex> lk(lock_);
		if (producer_ == producer && producer_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "SetProducer  producer_ == producer   !!! app_name =" << producer_->AppName() << ", stream_name = " << producer_->StreamName();
			//return;
		}
		if (producer_  )
		{ 
			//producer_->Close();
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << " Producer reset app_name =" << producer_->AppName() << ", stream_name = " << producer_->StreamName();
			producer_.reset();
			
		}
		if (producer)
		{
			producer_ = (producer);
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "add new  Producer   app_name =" << producer->AppName() << ", stream_name = " << producer->StreamName();
		}
		
	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 添加视频帧（Add Video Frame）
	 *  
	 *  接收来自生产者的视频帧，并异步分发给所有消费者。
	 *  
	 *  处理流程：
	 *  1. 接收编码后的视频帧（使用移动语义）
	 *  2. 可选：将视频帧保存到文件用于调试（通过宏控制）
	 *  3. 将分发任务投递到工作线程
	 *  4. 在工作线程中遍历所有消费者
	 *  5. 将视频帧发送给每个消费者
	 *  
	 *  @param frame 编码后的视频帧，使用移动语义避免拷贝
	 *  @note 该方法可能在生产者线程中调用
	 *  @note 实际分发在工作线程中异步执行
	 *  @note 支持调试模式，可以将视频帧保存到ps.h264文件
	 *  @note 使用lambda捕获移动后的帧，确保数据安全传递
	 */
	void Session::AddVideoFrame(  libmedia_codec::EncodedImage &&frame)
	{
#if 0
		static FILE * out_file_ptr = fopen("ps.h264", "wb+");
		if (out_file_ptr)
		{
			fwrite(frame.data(), frame.size(), 1, out_file_ptr);
			fflush(out_file_ptr);
		}
#endif //

		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, new_frame = std::move(frame)]() {
			for (auto consumer : consumers_)
			{
				consumer->AddVideoFrame(new_frame);
			}
		});
		
	}
	
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
	 *  @param pts 音频帧的显示时间戳
	 *  @note 该方法可能在生产者线程中调用
	 *  @note 实际分发在工作线程中异步执行
	 *  @note 使用lambda捕获移动后的帧和pts，确保数据安全传递
	 */
	void  Session::AddAudioFrame(  rtc::CopyOnWriteBuffer&& frame, int64_t pts)
	{
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, new_frame = std::move(frame), pts]() {
			for (auto consumer : consumers_)
			{
				consumer->AddAudioFrame(new_frame, pts);
			}
		});
		 
	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 添加数据通道消息（Add Data Channel）
	 *  
	 *  接收WebRTC DataChannel的数据消息，并异步分发给所有消费者和生产者。
	 *  
	 *  处理流程：
	 *  1. 接收SCTP流参数、PPID和消息数据
	 *  2. 将消息数据拷贝到rtc::Buffer中
	 *  3. 将分发任务投递到工作线程
	 *  4. 在工作线程中分发给所有消费者
	 *  5. 如果有生产者，也分发给生产者
	 *  
	 *  @param params SCTP流参数
	 *  @param ppid Payload Protocol Identifier
	 *  @param msg 消息数据指针
	 *  @param len 消息数据长度
	 *  @note 该方法可能在WebRTC线程中调用
	 *  @note 实际分发在工作线程中异步执行
	 *  @note 消息数据会被拷贝，原始数据可以安全释放
	 *  @note 支持双向数据通道，消费者和生产者都能收到消息
	 */
	void Session::AddDataChannel(
		const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, 
		uint32_t ppid, const uint8_t* msg, size_t len)
	{
		rtc::Buffer dataChannel(msg, len);
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, 
			[this, params, ppid, dataChannel_ = std::move(dataChannel)]() {
			/*for (auto consumer : consumers_)
			{
				consumer->OnDataChannel(params, ppid, 
					dataChannel_.data(), dataChannel_.size());
			}*/
			if (producer_)
			{
				producer_->OnDataChannel(params, ppid, dataChannel_.data(), dataChannel_.size());
			}
		});
	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 消费者请求关键帧（Consumer Request Key Frame）
	 *  
	 *  当消费者需要关键帧时，向生产者请求发送关键帧。
	 *  
	 *  处理流程：
	 *  1. 检查是否有生产者
	 *  2. 如果没有生产者，直接返回
	 *  3. 将请求任务投递到工作线程
	 *  4. 在工作线程中再次检查生产者是否存在
	 *  5. 调用生产者的RequestKeyFrame方法
	 *  
	 *  @note 该方法可能在消费者线程中调用
	 *  @note 实际请求在工作线程中异步执行
	 *  @note 双重检查生产者是否存在，避免空指针访问
	 *  @note 如果没有生产者，请求会被忽略
	 */
	void Session::ConsumerRequestKeyFrame()
	{

		if (!producer_)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "request KeyFrame failed !!! --> not find producer --->   session_name:" << session_name_;
			return;
		}
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE,
			[this]() {
				if (producer_)
				{
					producer_->RequestKeyFrame();
				}
		});
		
		
	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 获取流对象（Get Stream）
	 *  
	 *  返回会话关联的流对象。
	 *  
	 *  @return 返回流对象的共享指针
	 *  @note 流对象在会话构造时创建，通常不会为空
	 */
	std::shared_ptr<Stream> Session::GetStream()
	{
		return stream_;
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 获取会话名称（Session Name）
	 *  
	 *  返回会话的名称。
	 *  
	 *  @return 返回会话名称的常量引用
	 *  @note 返回引用避免字符串拷贝
	 */
	const std::string &Session::SessionName()const
	{
		return session_name_;
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 检查是否有生产者（Is Producer）
	 *  
	 *  检查会话是否已经设置了生产者。
	 *  
	 *  @return 如果有生产者返回true，否则返回false
	 *  @note 使用!!运算符将指针转换为bool值
	 *  @note 该方法不需要加锁，因为只是读取指针值
	 */
	bool Session::IsProducer() const
	{
		return !!producer_;
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 清理会话资源（Clear）
	 *  
	 *  清理会话的所有资源，包括生产者和所有消费者。
	 *  
	 *  清理流程：
	 *  1. 使用互斥锁保护资源
	 *  2. 如果有生产者，重置生产者对象
	 *  3. 遍历所有消费者，重置每个消费者对象
	 *  4. 清空消费者集合
	 *  
	 *  @note 该方法线程安全，使用互斥锁保护
	 *  @note 调用reset()会减少引用计数
	 *  @note 如果没有其他引用，对象会被自动销毁
	 *  @note 调用此方法后，会话将不再可用
	 */
	void Session::Clear()
	{
		std::lock_guard<std::mutex> lk(lock_);
		GBMEDIASERVER_LOG(LS_INFO) << " session_name = " << session_name_ << "clear !!!";
		if (producer_)
		{ 
			GBMEDIASERVER_LOG(LS_INFO) << " session_name = " << session_name_ << "clear !!!Producer   app_name =" << producer_->AppName() << ", stream_name = " << producer_->StreamName();
			producer_.reset();
			
		}
		for (  auto     p : consumers_)
		{ 
			p.reset();
			GBMEDIASERVER_LOG(LS_INFO) << " session_name = " << session_name_ << "clear !!! consumer   app_name =" << p->AppName() << ", stream_name = " << p->StreamName();
		}
		consumers_.clear();
	}
	 
	 
}
