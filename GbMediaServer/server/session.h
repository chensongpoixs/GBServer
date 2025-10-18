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
#ifndef _C_GB_MEDIA_SERVER_SESSIOIN_H____
#define _C_GB_MEDIA_SERVER_SESSIOIN_H____


#include "user/user.h"
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
//#include "user/player_user.h"
//#include "user/play_rtc_user.h"


namespace  gb_media_server
{
	class PlayerUser;
	//using PlayerUserPtr  = std::shared_ptr<PlayerUser> ;
	class User;
	//using UserPtr = std::shared_ptr<User>  ;
	class Connection;
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		explicit Session(const std::string & session_name);
		virtual	~Session();
	public:


		std::shared_ptr<User> CreatePublishUser(const std::shared_ptr<Connection> &conn,
			const std::string &session_name,
			const std::string &param,
			UserType type);
		std::shared_ptr<User> CreatePlayerUser(  std::shared_ptr<Connection> &conn,
			const std::string &session_name,
			const std::string &param,
			UserType type);
		void CloseUser(const std::shared_ptr<User> &user);
		void ActiveAllPlayers();
		void AddPlayer(const std::shared_ptr<PlayerUser>   &user);
		void SetPublisher(std::shared_ptr<User> &user);
		void  AddVideoFrame(const rtc::CopyOnWriteBuffer & frame);
	public:

		std::shared_ptr<Stream> GetStream();
		const std::string &SessionName()const;
		 
		bool IsPublishing() const;
		void Clear();
		int32_t ReadyTime() const;
		int64_t SinceStart() const;
		bool IsTimeout();
	private:
		void CloseUserNoLock(const std::shared_ptr<User> &user);
	private:
		std::string session_name_;

		//²¥·Å¶Ë
		std::unordered_set<std::shared_ptr<PlayerUser>>    players_;
		std::shared_ptr<Stream>							 stream_{ nullptr };
		//ÍÆÁ÷¶Ë
		std::shared_ptr<User>								 publisher_{ nullptr };
		std::mutex								lock_;
		std::atomic<int64_t>			     player_live_time_;

		//PullerRelay * pull_{ nullptr };
	};
}

#endif// 