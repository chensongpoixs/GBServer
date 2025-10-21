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
#ifndef _C_RTC_SERVICE_H_
#define _C_RTC_SERVICE_H_

#include "server/session.h"
#include "user/rtc_play_user.h"
#include "server/session.h"
#include "server/stream.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"
//#include "swagger/dto/RtcApiDto.hpp"
namespace  gb_media_server
{
	using   PlayRtcUserPtr = std::shared_ptr<PlayRtcUser>;
	class RtcService : public sigslot::has_slots<>
	{
	public:

		RtcService() ;
	virtual	~RtcService() ;
	public:
		static RtcService & GetInstance()
		{
			static RtcService   instance;
			return instance;
		}


		void AddPlayUser(PlayRtcUserPtr uesr);
		void RemovePlayUser(PlayRtcUserPtr uesr);


		webrtc::TaskQueueFactory*  GetTaskQueueFactory();
	//	oatpp::Object<RtcApiDto>   CreateOfferAnswer(const oatpp::Object<RtcApiDto>& dto);
	public:

		void OnStun(rtc::AsyncPacketSocket* socket,
			const char* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;
		void OnDtls(rtc::AsyncPacketSocket* socket,
			const char* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;
		void OnRtp(rtc::AsyncPacketSocket* socket,
			const char* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;
		void OnRtcp(rtc::AsyncPacketSocket* socket,
			const char* data,
			size_t len,
			const rtc::SocketAddress& addr,
			// TODO(bugs.webrtc.org/9584): Change to passing the int64_t
			// timestamp by value.
			const int64_t& ms) ;

	public:
		 
		
	public:


	//	static std::string GetSessionNameFromUrl(const std::string &url);
		std::mutex lock_;
		std::unordered_map<std::string, PlayRtcUserPtr> name_users_;
		std::mutex users_lock_;
		std::unordered_map<std::string, PlayRtcUserPtr> users_;
	public:
	private:
		std::unique_ptr<webrtc::TaskQueueFactory>                       task_queue_factory_;
	};
}


#endif // 