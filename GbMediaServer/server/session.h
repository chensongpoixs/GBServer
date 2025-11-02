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
		void SetProducer(std::shared_ptr<Producer> &producer);
		
		void  AddVideoFrame(  libmedia_codec::EncodedImage&&frame);
		 

		void  AddAudioFrame(  rtc::CopyOnWriteBuffer&& frame, int64_t pts);
	public:

		std::shared_ptr<Stream> GetStream();
		const std::string &SessionName()const;
		 
		bool IsProducer() const;
		void Clear();
	
	private: 
	private:
		std::string session_name_;

		//²¥·Å¶Ë
		std::unordered_set<std::shared_ptr<Consumer>>    consumers_;
		std::shared_ptr<Stream>							 stream_{ nullptr };
		//ÍÆÁ÷¶Ë
		 std::shared_ptr<Producer>								 producer_{ nullptr };
		std::mutex								lock_;
		std::atomic<int64_t>			     player_live_time_;

		//PullerRelay * pull_{ nullptr };
	};
}

#endif// 