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



namespace  gb_media_server
{
	namespace
	{
		static std::shared_ptr<Consumer> consumer_null;
		static std::shared_ptr<Producer> producer_null;
	}
	Session::Session(const std::string & session_name)
		:consumers_()
		, session_name_(session_name)
	{
		stream_ = std::make_shared<Stream>( *this, session_name);
		player_live_time_ = rtc::TimeMillis();
	}

	Session::~Session()
	{
		//if (pull_)
		//{
		//	delete pull_;
		//}
		consumers_.clear();
	}
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
	void Session::SetProducer(std::shared_ptr<Producer>  producer)
	{
		std::lock_guard<std::mutex> lk(lock_);
		if (producer_ == producer)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "SetProducer  producer_ == producer   !!!";
			//return;
		}
		if (producer_  )
		{ 
			//producer_->Close();
			producer_.reset();
			
		}
		if (producer)
		{
			producer_ = (producer);
		}
		
	}

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
	void  Session::AddAudioFrame(  rtc::CopyOnWriteBuffer&& frame, int64_t pts)
	{
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, new_frame = std::move(frame), pts]() {
			for (auto consumer : consumers_)
			{
				consumer->AddAudioFrame(new_frame, pts);
			}
		});
		 
	}

	void Session::AddDataChannel(
		const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, 
		uint32_t ppid, const uint8_t* msg, size_t len)
	{
		rtc::Buffer dataChannel(msg, len);
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, 
			[this, params, ppid, dataChannel_ = std::move(dataChannel)]() {
			for (auto consumer : consumers_)
			{
				consumer->OnDataChannel(params, ppid, 
					dataChannel_.data(), dataChannel_.size());
			}
			if (producer_)
			{
				producer_->OnDataChannel(params, ppid, dataChannel_.data(), dataChannel_.size());
			}
		});
	}

	void Session::ConsumerRequestKeyFrame()
	{

		if (!producer_)
		{
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

	std::shared_ptr<Stream> Session::GetStream()
	{
		return stream_;
	}
	const std::string &Session::SessionName()const
	{
		return session_name_;
	}
	
	bool Session::IsProducer() const
	{
		return !!producer_;
	}
	void Session::Clear()
	{
		std::lock_guard<std::mutex> lk(lock_);
		if (producer_)
		{ 
			producer_.reset();
		}
		for (  auto     p : consumers_)
		{ 
			p.reset();
		}
		consumers_.clear();
	}
	 
	 
}
