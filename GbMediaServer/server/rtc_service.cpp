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

#include "server/rtc_service.h"
  
#include "server/gb_media_service.h"
 
#include "rtc_base/string_encode.h"
 
#include "libmedia_transfer_protocol/librtc/stun.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "libmedia_transfer_protocol/libhttp/http_context.h"
#include "libmedia_transfer_protocol/libnetwork/tcp_session.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h"
#include "json/json.h"

#include "gb_media_server_log.h"

namespace  gb_media_server
{
	RtcService::RtcService()
		:  task_queue_factory_(webrtc::CreateDefaultTaskQueueFactory())
	{
		
	}
	RtcService::~RtcService()
	{
		
	}
 


	 void RtcService::RegisterRtcInterface(std::shared_ptr<RtcInterface> rtc_interface)
	 {
		 std::lock_guard<std::mutex> lk(lock_);
		 // TODO@chensong 2025-11-23 
		 // 如果多个连接使用相同的 `LocalUFrag`（虽然概率低），会导致冲突
		 //-后创建的连接会覆盖先创建的连接
		//	 - STUN 消息可能路由到错误的接口
		 name_rtc_interface_.emplace(rtc_interface->LocalUFrag(), rtc_interface);
	 }

	 void RtcService::UnregisterRtcInterface(std::shared_ptr<RtcInterface> rtc_interface)
	 {
		 std::lock_guard<std::mutex> lk(lock_);
		 name_rtc_interface_.erase(rtc_interface->LocalUFrag());
 
		 std::string key = rtc_interface->RtcRemoteAddress().ipaddr().ToString() + ":" + std::to_string(rtc_interface->RtcRemoteAddress().port());
		 rtc_interfaces_.erase(key);
 
	 }

	 webrtc::TaskQueueFactory * RtcService::GetTaskQueueFactory()
	 {
		 return task_queue_factory_.get();
	 }

	 void RtcService::OnStun(rtc::AsyncPacketSocket * socket, const uint8_t * data, size_t len,
		 const rtc::SocketAddress & addr, const int64_t & ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		libmedia_transfer_protocol::librtc::Stun  stun;
		if (!stun.Decode((const uint8_t *)data, len))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << " stun parse failed !!!" << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
			return;
		}
		std::shared_ptr< RtcInterface>  rtc_interface;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_rtc_interface_.find(stun.LocalUFrag());
		if (iter != name_rtc_interface_.end())
		{
			rtc_interface = iter->second;
			stun.SetPassword(rtc_interface->LocalPasswd());
			rtc_interface->SetRtcRemoteAddress(addr);
			stun.SetMessageType(libmedia_transfer_protocol::librtc::kStunMsgBindingResponse);
			uint32_t  mapped_addr = 0;

			//mapped_addr = libmedia_transfer_protocol::ByteReader<uint32_t>::ReadBigEndian(&(const uint8_t *)(&(addr.ipaddr().ipv4_address().S_un.S_addr)));
			stun.SetMappedAddr(ntohl(addr.ipaddr().ipv4_address().s_addr));
			stun.SetMappedPort(addr.port());

			rtc::Buffer packet = stun.Encode();
			  
			 socket->SendTo(packet.data(), packet.size(), addr, rtc::PacketOptions());
			 
		}
		else
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "not find  UFrag: "<< stun.LocalUFrag();
			//return;
		}


		if (rtc_interface)
		{ 
			std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
			auto iter1 = rtc_interfaces_.find(key);
			if (iter1 == rtc_interfaces_.end())
			{
				rtc_interfaces_.emplace(key, rtc_interface);
			} 
		}
		
	}
	void RtcService::OnDtls(rtc::AsyncPacketSocket * socket, const uint8_t * data, 
		size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		{
			std::lock_guard<std::mutex> lock(lock_);
			auto iter1 = rtc_interfaces_.find(key);
			if (iter1 != rtc_interfaces_.end())
			{
				iter1->second->OnDtlsRecv(data, len);
			}
			else
			{
				GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

			}
		}
	}
	void RtcService::OnRtp(rtc::AsyncPacketSocket * socket, const uint8_t * data, 
		size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		std::lock_guard<std::mutex> lock(lock_);
		auto iter1 = rtc_interfaces_.find(key);
		if (iter1 != rtc_interfaces_.end())
		{
			iter1->second->OnSrtpRtp((uint8_t *)data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}
	void RtcService::OnRtcp(rtc::AsyncPacketSocket * socket, const uint8_t * data,
		size_t len, const rtc::SocketAddress & addr, const int64_t & ms)
	{
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		std::lock_guard<std::mutex> lock(lock_);
		auto iter1 = rtc_interfaces_.find(key);
		if (iter1 != rtc_interfaces_.end())
		{
			iter1->second->OnSrtpRtcp((uint8_t *)data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}

	void RtcService::OnStun(rtc::Socket* socket, const uint8_t* data, size_t len,
		const rtc::SocketAddress& addr, const int64_t& ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		libmedia_transfer_protocol::librtc::Stun  stun;
		if (!stun.Decode((const uint8_t*)data, len))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << " stun parse failed !!!" << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
			return;
		}
		std::shared_ptr< RtcInterface>  rtc_interface;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_rtc_interface_.find(stun.LocalUFrag());
		if (iter != name_rtc_interface_.end())
		{
			rtc_interface = iter->second;
			stun.SetPassword(rtc_interface->LocalPasswd());
			rtc_interface->SetRtcRemoteAddress(addr);
			stun.SetMessageType(libmedia_transfer_protocol::librtc::kStunMsgBindingResponse);
			uint32_t  mapped_addr = 0;

			//mapped_addr = libmedia_transfer_protocol::ByteReader<uint32_t>::ReadBigEndian(&(const uint8_t *)(&(addr.ipaddr().ipv4_address().S_un.S_addr)));
			stun.SetMappedAddr(ntohl(addr.ipaddr().ipv4_address().s_addr));
			stun.SetMappedPort(addr.port());

			rtc::Buffer packet = stun.Encode();

			socket->SendTo(packet.data(), packet.size(), addr/*, rtc::PacketOptions()*/);

		}
		else
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "not find  UFrag: " << stun.LocalUFrag();
			//return;
		}


		if (rtc_interface)
		{
			std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
			auto iter1 = rtc_interfaces_.find(key);
			if (iter1 == rtc_interfaces_.end())
			{
				rtc_interfaces_.emplace(key, rtc_interface);
			}
		}

	}
	void RtcService::OnDtls(rtc::Socket* socket, const uint8_t* data,
		size_t len, const rtc::SocketAddress& addr, const int64_t& ms)
	{
		//GBMEDIASERVER_LOG_F(LS_INFO) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << addr.ToString();
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		{
			std::lock_guard<std::mutex> lock(lock_);
			auto iter1 = rtc_interfaces_.find(key);
			if (iter1 != rtc_interfaces_.end())
			{
				iter1->second->OnDtlsRecv(data, len);
			}
			else
			{
				GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

			}
		}
	}
	void RtcService::OnRtp(rtc::Socket* socket, const uint8_t* data,
		size_t len, const rtc::SocketAddress& addr, const int64_t& ms)
	{
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		std::lock_guard<std::mutex> lock(lock_);
		auto iter1 = rtc_interfaces_.find(key);
		if (iter1 != rtc_interfaces_.end())
		{
			iter1->second->OnSrtpRtp((uint8_t*)data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}
	void RtcService::OnRtcp(rtc::Socket* socket, const uint8_t* data,
		size_t len, const rtc::SocketAddress& addr, const int64_t& ms)
	{
		std::string key = addr.ipaddr().ToString() + ":" + std::to_string(addr.port());
		std::lock_guard<std::mutex> lock(lock_);
		auto iter1 = rtc_interfaces_.find(key);
		if (iter1 != rtc_interfaces_.end())
		{
			iter1->second->OnSrtpRtcp((uint8_t*)data, len);
		}
		else
		{
			GBMEDIASERVER_LOG_F(LS_WARNING) << "local:" << socket->GetLocalAddress().ToString() << ", remote:" << key;

		}
	}
	 
	
}