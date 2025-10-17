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
#include "user/user.h"
#include "user/play_rtc_user.h"
#include "user/player_user.h"
#include "rtc_base/time_utils.h"
#include "rtc_base/string_utils.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_encode.h"
#include <string>
#include "absl/strings/string_view.h"
#include "server/stream.h"
#include "server/connection.h"
#include "server/stream.h"
#include "user/play_rtc_user.h"
#include <memory>
#include "user/user.h"
#include "api/array_view.h"
#include "utils/time_corrector.h"
#include "absl/strings/string_view.h"
namespace  gb_media_server
{
	namespace
	{
		static std::shared_ptr<User> user_null;
	}
	Session::Session(const std::string & session_name)
		:players_()
		, session_name_(session_name)
		//, stream_(nullptr, "")
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
		players_.clear();
	}
	std::shared_ptr<User> Session::CreatePublishUser(const std::shared_ptr<Connection> &conn,
		const std::string &session_name,
		const std::string &param,
		UserType type)
	{
		if (session_name != session_name_)
		{
			 
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish user failed !!! invalid session name: " << session_name;
			return user_null;
		}
		//std::vector<std::string> list = tmms::base::StringUtils::SplitString(session_name, "/");
		std::vector<std::string> list;
		split(session_name, '/', & list);

		std::shared_ptr<User> user = std::make_shared<User>(conn, stream_, shared_from_this());
		//user->SetAppInfo(app_info_);
		//user->SetDomainName(list[0]);
		user->SetAppName(list[1]);
		user->SetStreamName(list[2]);
		user->SetParam(param);
		user->SetUserType(type);

		conn->SetContext(kUserContext, user);
		return user;
	}
	std::shared_ptr<User> Session::CreatePlayerUser(  std::shared_ptr<Connection> &conn,
		const std::string &session_name,
		const std::string &param,
		UserType type)
	{
		if (session_name != session_name_)
		{
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish user failed.Invalid session name:" << session_name;
			return user_null;
		}
		//auto list = base::StringUtils::SplitString(session_name, "/");
		std::vector<std::string> list;
		//std::string p = session_name;
		//absl::string_view  p();
		//std::string p = rtc::hex_encode(session_name);
		//GBMEDIASERVER_LOG(LS_INFO) << p;
		split(/*rtc::ArrayView*/( session_name ), '/', &list);
		if (list.size() != 3)
		{
			GBMEDIASERVER_LOG(LS_ERROR) << "create publish user failed.Invalid session name:" << session_name;
			return user_null;
		}
		std::shared_ptr< PlayerUser> user;
		if (type == UserType::kUserTypePlayerWebRTC)
		{
			user = std::make_shared<PlayRtcUser>(conn, stream_, shared_from_this());
		} 
		else
		{
			return user_null;
		}
		//user->SetAppInfo(app_info_);
		//user->SetDomainName(list[0]);
		user->SetAppName(list[1]);
		user->SetStreamName(list[2]);
		user->SetParam(param);
		user->SetUserType(type);
		conn->SetContext(kUserContext, user);

		return user;
	}
	void Session::CloseUser(const std::shared_ptr<User> &user)
	{
		if (!user->destroyed_.exchange(true))
		{
			{
				std::lock_guard<std::mutex> lk(lock_);
				// 类型错误导致释放对象错误了 修复bug 
				if (user->GetUserType() <= UserType::kUserTypePublishWebRtc)
				{
					if (publisher_)
					{
						GBMEDIASERVER_LOG(INFO) << "remove publisher,session name:" << session_name_
							<< ",user:" << user->UserId()
							<< ",elapsed:" << user->ElapsedTime()
							<< ",ReadyTime:" << ReadyTime()
							<< ",stream time:" << SinceStart();

						publisher_.reset();
					}
				}
				else
				{
					GBMEDIASERVER_LOG(INFO) << "remove player,session name:" << session_name_
						<< ",user:" << user->UserId()
						<< ",elapsed:" << user->ElapsedTime()
						<< ",ReadyTime:" << ReadyTime()
						<< ",stream time:" << SinceStart();
					players_.erase(std::dynamic_pointer_cast<PlayerUser>(user));
					player_live_time_ = rtc::TimeMillis();
				}
			}
			user->Close();
		}
	}
	void Session::ActiveAllPlayers()
	{
		std::lock_guard<std::mutex> lk(lock_);
		for (auto const &u : players_)
		{
			u->Active();
		}
	}
	void Session::AddPlayer(const std::shared_ptr< PlayerUser> &user)
	{
		{
			std::lock_guard<std::mutex> lk(lock_);
			players_.insert(user);
		}



		if (!publisher_)
		{
			GBMEDIASERVER_LOG(INFO) << " add player,  realy  -->  session name:" << session_name_ << ",user:" << user->UserId();

			//if (!pull_)
			//{
			//	pull_ = new PullerRelay(*this);
			//}
			//pull_->StartPullStream();
		}
		else
		{
			GBMEDIASERVER_LOG(INFO) << " add player,  local stream   -->  session name:" << session_name_ << ",user:" << user->UserId();

		}
		user->Active();
	}
	void Session::SetPublisher(std::shared_ptr<User> &user)
	{
		std::lock_guard<std::mutex> lk(lock_);
		if (publisher_ == user)
		{
			return;
		}
		if (publisher_ && !publisher_->destroyed_.exchange(true))
		{
			publisher_->Close();
		}
		publisher_ = (user);
	}

	std::shared_ptr<Stream> Session::GetStream()
	{
		return stream_;
	}
	const std::string &Session::SessionName()const
	{
		return session_name_;
	}
	
	bool Session::IsPublishing() const
	{
		return !!publisher_;
	}
	void Session::Clear()
	{
		std::lock_guard<std::mutex> lk(lock_);
		if (publisher_)
		{
			CloseUserNoLock(publisher_);
		}
		for (auto const   &p : players_)
		{
			CloseUserNoLock( (p));
		}
		players_.clear();
	}
	int32_t Session::ReadyTime() const
	{
		return stream_->ReadyTime();
	}
	int64_t Session::SinceStart() const
	{
		return stream_->SinceStart();
	}
	bool Session::IsTimeout()
	{
		if (stream_->Timeout())
		{
			return true;
		}

#if PLAYER_LIVE_TIMEOUT
		// 空闲没有播放用户时关闭
		int64_t idle = tmms::base::TTime::NowMS() - player_live_time_;
		if (players_.empty() && idle > app_info_->stream_idle_time_)
		{
			return true;
		}
#endif 
		return false;
	}
	void Session::CloseUserNoLock(const std::shared_ptr<User> &user)
	{
		if (!user->destroyed_.exchange(true))
		{
			{
				if (user->GetUserType() <= UserType::kUserTypePublishWebRtc)
				{
					if (publisher_)
					{
						GBMEDIASERVER_LOG(INFO) << "remove publisher,session name:" << session_name_
							<< ",user:" << user->UserId()
							<< ",elapsed:" << user->ElapsedTime()
							<< ",ReadyTime:" << ReadyTime()
							<< ",stream time:" << SinceStart();
						user->Close();
						publisher_.reset();
					}
				}
				else
				{
					GBMEDIASERVER_LOG(INFO) << "remove player,session name:" << session_name_
						<< ",user:" << user->UserId()
						<< ",elapsed:" << user->ElapsedTime()
						<< ",ReadyTime:" << ReadyTime()
						<< ",stream time:" << SinceStart();
					//players_.erase(std::dynamic_pointer_cast<PlayerUser>(user));
					user->Close();
					player_live_time_ = rtc::TimeMillis();
				}
			}

		}
	}
}
