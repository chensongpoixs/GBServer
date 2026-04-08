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

#include "producer/rtc_producer.h"
#include "api/task_queue/default_task_queue_factory.h"

namespace  gb_media_server
{
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 构造函数（Constructor）
	 *  
	 *  初始化RTC服务实例，创建任务队列工厂。
	 *  
	 *  初始化流程：
	 *  1. 创建WebRTC默认任务队列工厂
	 *  2. 初始化RTC接口映射表
	 *  3. 初始化互斥锁
	 *  
	 *  @note 任务队列工厂用于创建异步任务队列
	 *  @note 使用WebRTC库提供的默认实现
	 */
	RtcService::RtcService()
		:  task_queue_factory_(webrtc::CreateDefaultTaskQueueFactory())
		, name_rtc_ufrag_()
	{
		
	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 析构函数（Destructor）
	 *  
	 *  清理RTC服务资源。
	 *  
	 *  @note 任务队列工厂由unique_ptr自动释放
	 *  @note RTC接口映射表会自动清空
	 */
	RtcService::~RtcService()
	{
		
	}
	 
	 /***
	  *  @author chensong
	  *  @date 2025-10-18
	  *  @brief 注册RTC接口（Register RTC Interface）
	  *  
	  *  将RTC接口注册到服务中，使其能够接收数据包。
	  *  
	  *  注册流程：
	  *  1. 使用互斥锁保护映射表
	  *  2. 使用LocalUFrag作为键，将接口添加到映射表
	  *  3. LocalUFrag是ICE协议中的用户名片段，用于标识连接
	  *  
	  *  @param rtc_interface RTC接口的共享指针
	  *  @note 该方法线程安全
	  *  @note TODO: 如果多个连接使用相同的LocalUFrag会导致冲突
	  *  @note 后创建的连接会覆盖先创建的连接
	  */
	 void RtcService::RegisterRtcInterface(std::shared_ptr<RtcInterface> rtc_interface)
	 {
		 std::string ufrag_name = rtc_interface->LocalUFrag();// +":" + rtc_interface->LocalPasswd();
		 {
			 std::lock_guard<std::mutex> lk(lock_);
			 // TODO@chensong 2025-11-23 
			 // 如果多个连接使用相同的 `LocalUFrag`（虽然概率低），会导致冲突
			 //-后创建的连接会覆盖先创建的连接
			//	 - STUN 消息可能路由到错误的接口
			 name_rtc_interface_.emplace(ufrag_name, rtc_interface);
		 }
		 name_rtc_ufrag_.emplace(ufrag_name);
		 GBMEDIASERVER_LOG(LS_INFO) << "add rtc ufrag_name = " << ufrag_name ;
	 }

	 /***
	  *  @author chensong
	  *  @date 2025-10-18
	  *  @brief 注销RTC接口（Unregister RTC Interface）
	  *  
	  *  从服务中注销RTC接口，停止向其路由数据包。
	  *  
	  *  注销流程：
	  *  1. 使用互斥锁保护映射表
	  *  2. 从name_rtc_interface_映射表中移除（按LocalUFrag索引）
	  *  3. 从rtc_interfaces_映射表中移除（按远程地址索引）
	  *  4. 构造远程地址键：IP:Port格式
	  *  
	  *  @param rtc_interface RTC接口的共享指针
	  *  @note 该方法线程安全
	  *  @note 需要同时从两个映射表中移除
	  */
	 void RtcService::UnregisterRtcInterface(std::shared_ptr<RtcInterface> rtc_interface)
	 {
		 std::string key = rtc_interface->RtcRemoteAddress().ipaddr().ToString() + ":" + std::to_string(rtc_interface->RtcRemoteAddress().port());
		 std::string ufrag_name = rtc_interface->LocalUFrag();// +":" + rtc_interface->LocalPasswd();
		 {
			 std::lock_guard<std::mutex> lk(lock_);
			 name_rtc_interface_.erase(ufrag_name);


			 rtc_interfaces_.erase(key);
		 }
		 name_rtc_ufrag_.erase(ufrag_name);
		 GBMEDIASERVER_LOG(LS_INFO) << "remove rtc ufrag_name = "<< ufrag_name << ", key = " << key;
 
	 }
	 void RtcService::UnregisterRtcInterface(const std::string& ufrag_name, const std::string& key)
	 {
		 {
			 std::lock_guard<std::mutex> lk(lock_);
			 name_rtc_interface_.erase(ufrag_name);


			 rtc_interfaces_.erase(key);
		 }
		 name_rtc_ufrag_.erase(ufrag_name);
		 GBMEDIASERVER_LOG(LS_INFO) << "remove rtc ufrag_name = " << ufrag_name << ", key = " << key;
	 }
	 bool RtcService::FindUfragName(const std::string& ufrag_name)
	 {
		 auto iter =  name_rtc_ufrag_.find(ufrag_name);
		 if (iter != name_rtc_ufrag_.end())
		 {
			 return true;
		 }
		 return false;
	 }
	 /***
	  *  @author chensong
	  *  @date 2025-10-18
	  *  @brief 获取任务队列工厂（Get Task Queue Factory）
	  *  
	  *  返回任务队列工厂指针，供RTC接口创建任务队列。
	  *  
	  *  @return 返回任务队列工厂指针
	  *  @note 任务队列工厂在构造函数中创建
	  *  @note 返回的指针由服务管理，不需要手动释放
	  */
	 webrtc::TaskQueueFactory * RtcService::GetTaskQueueFactory()
	 {
		 return task_queue_factory_.get();
	 }

	 /***
	  *  @author chensong
	  *  @date 2025-10-18
	  *  @brief 处理STUN数据包（UDP版本）（On STUN Packet - UDP）
	  *  
	  *  处理来自UDP套接字的STUN数据包，实现ICE连接建立。
	  *  
	  *  处理流程：
	  *  1. 解析STUN数据包，提取LocalUFrag
	  *  2. 根据LocalUFrag查找对应的RTC接口
	  *  3. 如果找到接口：
	  *     - 设置STUN密码
	  *     - 更新RTC接口的远程地址
	  *     - 构造STUN Binding Response
	  *     - 设置Mapped Address（客户端的公网地址）
	  *     - 发送响应给客户端
	  *     - 将接口添加到rtc_interfaces_映射表（按远程地址索引）
	  *  4. 如果未找到接口，记录警告日志
	  *  
	  *  @param socket UDP套接字指针
	  *  @param data STUN数据包指针
	  *  @param len 数据包长度
	  *  @param addr 远程地址
	  *  @param ms 时间戳
	  *  @note 该方法在网络线程中调用
	  *  @note STUN响应包含客户端的公网地址，用于NAT穿透
	  */
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
		std::string ufrag_name = stun.LocalUFrag();// +":" + stun.PassWord();;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_rtc_interface_.find(ufrag_name);
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
			if (rtc_interface->GetSdpType() == libmedia_transfer_protocol::librtc::kRtcSdpPlay)
			{
				std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(rtc_interface);
				slef->SetStunTime();
			}
			else if (rtc_interface->GetSdpType() == libmedia_transfer_protocol::librtc::kRtcSdpPush)
			{
				std::shared_ptr<RtcProducer> slef = std::dynamic_pointer_cast<RtcProducer>(rtc_interface);
				slef->SetStunTime();
			}
			//rtc_interface->SetStunTime();
			 socket->SendTo(packet.data(), packet.size(), addr, rtc::PacketOptions());
			 
		}
		else
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "not find  UFrag: "<< ufrag_name;
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理DTLS数据包（UDP版本）（On DTLS Packet - UDP）
	 *  
	 *  处理来自UDP套接字的DTLS数据包，实现密钥交换和握手。
	 *  
	 *  处理流程：
	 *  1. 构造远程地址键（IP:Port格式）
	 *  2. 使用互斥锁保护映射表
	 *  3. 根据远程地址查找对应的RTC接口
	 *  4. 如果找到接口，调用其OnDtlsRecv方法处理DTLS数据
	 *  5. 如果未找到接口，记录警告日志
	 *  
	 *  @param socket UDP套接字指针
	 *  @param data DTLS数据包指针
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note DTLS握手完成后会建立SRTP加密会话
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理RTP数据包（UDP版本）（On RTP Packet - UDP）
	 *  
	 *  处理来自UDP套接字的RTP数据包，传输音视频媒体数据。
	 *  
	 *  处理流程：
	 *  1. 构造远程地址键（IP:Port格式）
	 *  2. 使用互斥锁保护映射表
	 *  3. 根据远程地址查找对应的RTC接口
	 *  4. 如果找到接口，调用其OnSrtpRtp方法处理RTP数据
	 *  5. 如果未找到接口，记录警告日志
	 *  
	 *  @param socket UDP套接字指针
	 *  @param data RTP数据包指针（可能是SRTP加密的）
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note RTP数据包通常经过SRTP加密，需要解密后才能使用
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理RTCP数据包（UDP版本）（On RTCP Packet - UDP）
	 *  
	 *  处理来自UDP套接字的RTCP数据包，传输控制信息。
	 *  
	 *  处理流程：
	 *  1. 构造远程地址键（IP:Port格式）
	 *  2. 使用互斥锁保护映射表
	 *  3. 根据远程地址查找对应的RTC接口
	 *  4. 如果找到接口，调用其OnSrtpRtcp方法处理RTCP数据
	 *  5. 如果未找到接口，记录警告日志
	 *  
	 *  @param socket UDP套接字指针
	 *  @param data RTCP数据包指针（可能是SRTCP加密的）
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note RTCP数据包通常经过SRTCP加密，需要解密后才能使用
	 */
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

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理STUN数据包（TCP版本）（On STUN Packet - TCP）
	 *  
	 *  处理来自TCP套接字的STUN数据包，实现ICE连接建立。
	 *  
	 *  处理流程：
	 *  1. 解析STUN数据包，提取LocalUFrag
	 *  2. 根据LocalUFrag查找对应的RTC接口
	 *  3. 如果找到接口：
	 *     - 设置STUN密码
	 *     - 更新RTC接口的远程地址
	 *     - 构造STUN Binding Response
	 *     - 设置Mapped Address（客户端的公网地址）
	 *     - 发送响应给客户端（使用TCP套接字）
	 *     - 将接口添加到rtc_interfaces_映射表（按远程地址索引）
	 *  4. 如果未找到接口，记录警告日志
	 *  
	 *  TCP与UDP的区别：
	 *  - TCP版本使用rtc::Socket而非rtc::AsyncPacketSocket
	 *  - TCP版本的SendTo不需要PacketOptions参数
	 *  - TCP提供可靠传输，适用于防火墙限制UDP的场景
	 *  
	 *  @param socket TCP套接字指针
	 *  @param data STUN数据包指针
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note TCP版本的STUN处理流程与UDP版本基本相同
	 *  @note 主要用于某些特殊网络环境（如UDP被阻止）
	 */
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
		std::string ufrag_name = stun.LocalUFrag();// +":" + stun.PassWord();;
		std::lock_guard<std::mutex> lk(lock_);
		auto iter = name_rtc_interface_.find(ufrag_name);
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
			GBMEDIASERVER_LOG(LS_WARNING) << "not find  UFrag: " << ufrag_name;
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理DTLS数据包（TCP版本）（On DTLS Packet - TCP）
	 *  
	 *  处理来自TCP套接字的DTLS数据包，实现密钥交换和握手。
	 *  
	 *  处理流程：
	 *  1. 构造远程地址键（IP:Port格式）
	 *  2. 使用互斥锁保护映射表
	 *  3. 根据远程地址查找对应的RTC接口
	 *  4. 如果找到接口，调用其OnDtlsRecv方法处理DTLS数据
	 *  5. 如果未找到接口，记录警告日志
	 *  
	 *  TCP与UDP的区别：
	 *  - TCP版本使用rtc::Socket而非rtc::AsyncPacketSocket
	 *  - TCP提供可靠传输，确保DTLS握手数据不丢失
	 *  - 适用于防火墙限制UDP的场景
	 *  
	 *  @param socket TCP套接字指针
	 *  @param data DTLS数据包指针
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note DTLS握手完成后会建立SRTP加密会话
	 *  @note TCP版本的DTLS处理流程与UDP版本相同
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理RTP数据包（TCP版本）（On RTP Packet - TCP）
	 *  
	 *  处理来自TCP套接字的RTP数据包，传输音视频媒体数据。
	 *  
	 *  处理流程：
	 *  1. 构造远程地址键（IP:Port格式）
	 *  2. 使用互斥锁保护映射表
	 *  3. 根据远程地址查找对应的RTC接口
	 *  4. 如果找到接口，调用其OnSrtpRtp方法处理RTP数据
	 *  5. 如果未找到接口，记录警告日志
	 *  
	 *  TCP与UDP的区别：
	 *  - TCP版本使用rtc::Socket而非rtc::AsyncPacketSocket
	 *  - TCP提供可靠传输，避免RTP包丢失
	 *  - TCP传输RTP会增加延迟，但提高可靠性
	 *  - 适用于防火墙限制UDP的场景
	 *  
	 *  @param socket TCP套接字指针
	 *  @param data RTP数据包指针（可能是SRTP加密的）
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note RTP数据包通常经过SRTP加密，需要解密后才能使用
	 *  @note TCP版本的RTP处理流程与UDP版本相同
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理RTCP数据包（TCP版本）（On RTCP Packet - TCP）
	 *  
	 *  处理来自TCP套接字的RTCP数据包，传输控制信息。
	 *  
	 *  处理流程：
	 *  1. 构造远程地址键（IP:Port格式）
	 *  2. 使用互斥锁保护映射表
	 *  3. 根据远程地址查找对应的RTC接口
	 *  4. 如果找到接口，调用其OnSrtpRtcp方法处理RTCP数据
	 *  5. 如果未找到接口，记录警告日志
	 *  
	 *  TCP与UDP的区别：
	 *  - TCP版本使用rtc::Socket而非rtc::AsyncPacketSocket
	 *  - TCP提供可靠传输，确保RTCP控制信息不丢失
	 *  - 适用于防火墙限制UDP的场景
	 *  
	 *  @param socket TCP套接字指针
	 *  @param data RTCP数据包指针（可能是SRTCP加密的）
	 *  @param len 数据包长度
	 *  @param addr 远程地址
	 *  @param ms 时间戳
	 *  @note 该方法在网络线程中调用
	 *  @note RTCP数据包通常经过SRTCP加密，需要解密后才能使用
	 *  @note TCP版本的RTCP处理流程与UDP版本相同
	 */
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