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



 ******************************************************************************/

#include "server/session.h"
#include "rtc_base/logging.h" 
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
#include "utils/time_corrector.h"
#include "absl/strings/string_view.h"
#include "utils/string_utils.h" 
#include "consumer/rtc_play_consumer.h"
#include "producer/gb28181_push_producer.h"
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
		ProducerType type)
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
			case gb_media_server::kProducerTypePublishGB28181:
			{
				producer = std::make_shared<Gb28181PushProducer>( stream_, shared_from_this());
				break;
			}
			 
			default:
			{
				return producer_null; 
				break;
			}
		}
		 
		producer->SetAppName(list[1]);
		producer->SetStreamName(list[2]);
		producer->SetParam(param);
		producer->SetProducerType(type);

		//conn->SetContext(kUserContext, producer);
		return producer;
	}
	std::shared_ptr<Consumer> Session::CreateConsumer(   
		const std::string &session_name,
		const std::string &param,
		ConsumerType type)
	{
		if (session_name != session_name_)
		{
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish Consumer failed.Invalid session name:" << session_name;
			return consumer_null;
		}
		//auto list = base::StringUtils::SplitString(session_name, "/");
		std::vector<std::string> list;
		//std::string p = session_name;
		//absl::string_view  p();
		//std::string p = rtc::hex_encode(session_name);
		//GBMEDIASERVER_LOG(LS_INFO) << p;
		string_utils:: split(/*rtc::ArrayView*/( session_name ), '/', &list);
		if (list.size() != 3)
		{
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish user failed.Invalid session name:" << session_name;
			return consumer_null;
		}
		std::shared_ptr< Consumer> consumer;
		if (type == ConsumerType::kConsumerTypePlayerWebRTC)
		{
			consumer = std::make_shared<RtcPlayConsumer>(  stream_, shared_from_this());
		} 
		else
		{
			return consumer_null;
		}
		//user->SetAppInfo(app_info_);
		//user->SetDomainName(list[0]);
		consumer->SetAppName(list[1]);
		consumer->SetStreamName(list[2]);
		consumer->SetParam(param);
		consumer->SetConsumerType(type);
		 
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
			GBMEDIASERVER_LOG(INFO) << " add consumer,  realy  -->  session name:" << session_name_ << ",consumer id:" << consumer->GetRemoteAddress().ToString();

			//if (!pull_)
			//{
			//	pull_ = new PullerRelay(*this);
			//}
			//pull_->StartPullStream();
		}
		else
		{
			GBMEDIASERVER_LOG(INFO) << " add consumer,  local stream   -->  session name:" << session_name_ << ",consumer id:" << consumer->GetRemoteAddress().ToString();

		}
		//consumer->Active();
	}
	 
	void Session::RemoveConsumer(const std::shared_ptr<Consumer> & consumer)
	{
		 
			{
				std::lock_guard<std::mutex> lk(lock_);
				// 类型错误导致释放对象错误了 修复bug 
				
				{
					GBMEDIASERVER_LOG(INFO) << "remove consumer,session name:" << session_name_
						<< ",remoteaddr:" << consumer->GetRemoteAddress().ToString()
						//<< ",elapsed:" << consumer->ElapsedTime()
						//<< ",ReadyTime:" << ReadyTime()
						//<< ",stream time:" << SinceStart() 
						<< ", use_count: " << consumer.use_count();
					consumers_.erase(consumer);
					player_live_time_ = rtc::TimeMillis();
				}
			}
		//	consumer->Close();
		 
	}
	void Session::SetProducer(std::shared_ptr<Producer> &producer)
	{
		std::lock_guard<std::mutex> lk(lock_);
		if (producer_ == producer)
		{
			return;
		}
		if (producer_  )
		{ 
			//producer_->Close();
			producer_.reset();
		}
		producer_ = (producer);
	}

	void Session::AddVideoFrame(const libmedia_codec::EncodedImage &frame)
	{
#if 0
		static FILE * out_file_ptr = fopen("ps.h264", "wb+");
		if (out_file_ptr)
		{
			fwrite(frame.data(), frame.size(), 1, out_file_ptr);
			fflush(out_file_ptr);
		}
#endif //
		for (auto consumer : consumers_)
		{ 
			consumer->OnVideoFrame(frame);
		}
	}
	void  Session::AddAudioFrame(const rtc::CopyOnWriteBuffer& frame)
	{

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
			//CloseUserNoLock(publisher_);
			producer_.reset();
		}
		for (  auto     p : consumers_)
		{
			//CloseUserNoLock( (p));
			p.reset();
		}
		consumers_.clear();
	}
	 
	 
}
