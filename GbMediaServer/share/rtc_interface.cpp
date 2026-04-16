/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-11-03

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

#include "share/rtc_interface.h"

#include <random>
#include "consumer/rtc_consumer.h"
#include "server/session.h"
#include "server/stream.h"
#include "rtc_base/logging.h"
#include "rtc_base/buffer.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format_h264.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include <vector>
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format.h"
#include <memory>
#include "server/rtc_service.h"
#include "common_video/h264/h264_common.h"
#include "libmedia_transfer_protocol/librtc/rtc_errors.h"
#include "gb_media_server_log.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/receiver_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/psfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sender_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sdes.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/extended_reports.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/bye.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/rtpfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/pli.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/librtcp/twcc_context.h"
#include "libmedia_transfer_protocol/librtcp/rtcp.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_feedback.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_context.h"
#include "rtc_base/string_utils.h"
#include "rtc_base/string_encode.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
#include <iterator>
#include <algorithm> // for std::lower_bound (on iterators)
#include <iterator> // std::ostream_iterator
#include <sstream>  // std::ostringstream
#include <utility>  // std::make_pair()


#include "consumer/rtc_consumer.h"
#include "server/session.h"

#include "gb_media_server_log.h"
#include "rtc_base/buffer.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format_h264.h"
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_to_send.h"
#include <vector>
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_format.h"
#include <memory>
#include "server/rtc_service.h"
#include "common_video/h264/h264_common.h"
#include "server/stream.h"
#include "utils/yaml_config.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/receiver_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/psfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sender_report.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/sdes.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/extended_reports.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/bye.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/rtpfb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/pli.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/compound_packet.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/fir.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/nack.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/remb.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/transport_feedback.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/tmmbn.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/tmmbr.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/tmmb_item.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/rapid_resync_request.h"
#include "share/statistics_manager.h"
#include "gb_media_server_log.h"
#include "producer/rtc_producer.h"


namespace gb_media_server
{


	namespace {
		/**
		*  @brief 最大视频包缓存数量
		*  
		*  该常量定义了视频RTP包缓存的最大数量，用于NACK重传。
		*  当缓存的RTP包数量超过该值时，会删除最旧的包。
		*  
		*  @note 1000个包约占用1-2MB内存（假设每个包1-2KB）
		*  @note 缓存过多会占用内存，缓存过少会导致NACK重传失败
		*/
		static const uint32_t    kMaxVideoPacketSize = 1000;
	}
 

#if 0

	void RtcInterface::OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO) << "DTLS connecting";
	}

	void RtcInterface::OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
		libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
		uint8_t* srtpLocalKey,
		size_t srtpLocalKeyLen,
		uint8_t* srtpRemoteKey,
		size_t srtpRemoteKeyLen,
		std::string& remote_cert)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "DTLS connected";



		delete srtp_send_session_;
		srtp_send_session_ = nullptr;
		delete srtp_recv_session_;
		srtp_recv_session_ = nullptr;


		try
		{
			srtp_send_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::OUTBOUND, srtpCryptoSuite, srtpLocalKey, srtpLocalKeyLen);
		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR) << "error creating SRTP sending session: " << error.what();
		}
		try
		{
			srtp_recv_session_ = new libmedia_transfer_protocol::libsrtp::SrtpSession(
				libmedia_transfer_protocol::libsrtp::INBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);

		}
		catch (const std::runtime_error& error)
		{
			GBMEDIASERVER_LOG_T_F(LS_ERROR) << "error creating SRTP receiving session: %s", error.what();

			delete srtp_send_session_;
			srtp_send_session_ = nullptr;
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "dtls handshake done.";
		dtls_done_ = true;
		//srtp_session_.Init(dtls_.RecvKey(), dtls_.SendKey());
		// return;
		 // 完成验证后进行发送
		// sctp 
		// sctp 

		sctp_ = std::make_shared<libmedia_transfer_protocol::librtc::SctpAssociationImp>(GbMediaService::GetInstance().worker_thread(), this, 128, 128, 262144, true);
		sctp_->TransportConnected();



		StartCapture();
	}
	void RtcInterface::OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "dtls send size:" << len;

		rtc::Buffer buffer(data, len);
		GbMediaService::GetInstance().GetRtcServer()->SendPacketTo(std::move(buffer), rtc_remote_address_, rtc::PacketOptions());
	}
	//void OnDtlsHandshakeDone(libmedia_transfer_protocol::libssl::Dtls *dtls);
	void RtcInterface::OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS remotely closed";
		dtls_done_ = false;

		StopCapture();

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().RemoveConsumer(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);
			});
		// 
	}
	void RtcInterface::OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls)
	{
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS failed";
		dtls_done_ = false;

		StopCapture();

		std::string session_name = GetSession()->SessionName();
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [this, session_name]() {
			std::shared_ptr<RtcConsumer> slef = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
			RtcService::GetInstance().RemoveConsumer(slef);
			//GbMediaService::GetInstance().CloseSession(session_name);
			GetSession()->RemoveConsumer(slef);

			});
		// 
	}
	void RtcInterface::OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len)
	{
		// Pass it to the parent transport.
		GBMEDIASERVER_LOG(LS_WARNING) << "DTLS application data recice data ";
		if (sctp_)
		{
			//sctp_
		}
	}
  
#endif // 0



	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC接口构造函数（RTC Interface Constructor）
	*  
	*  该构造函数用于初始化RTC接口的所有组件，包括DTLS、SRTP、RTP扩展、TWCC上下文等。
	*  RTC接口是RTC生产者和RTC消费者的基类，提供WebRTC通信的核心功能。
	*  
	*  初始化流程：
	*  1. 初始化DTLS对象，传入任务队列工厂
	*  2. 初始化SRTP发送和接收会话为nullptr
	*  3. 初始化RTP头部和扩展管理器
	*  4. 初始化TWCC上下文，用于带宽估计
	*  5. 生成本地ICE用户名片段（8字符）和密码（32字符）
	*  6. 设置SDP的本地ICE用户名和密码
	*  7. 注册RTP头部扩展：
	*     - AbsoluteSendTime：绝对发送时间扩展
	*     - TransportSequenceNumber：传输序列号扩展
	*  8. 设置TWCC上下文的回调函数，用于发送TWCC反馈
	*  
	*  RTP头部扩展：
	*  - AbsoluteSendTime：用于精确的时间同步，24位时间戳
	*  - TransportSequenceNumber：用于带宽估计和拥塞控制，16位序列号
	*  
	*  TWCC（Transport Wide Congestion Control）：
	*  - TWCC用于带宽估计和拥塞控制
	*  - 通过传输序列号扩展标识RTP包的发送顺序
	*  - 接收端记录RTP包的接收时间，生成TWCC反馈
	*  - 发送端根据TWCC反馈调整发送码率
	*  
	*  @note ICE用户名和密码用于ICE连接检查的认证
	*  @note RTP扩展必须在SDP交换前注册
	*  @note TWCC回调函数会在接收到足够的RTP包后触发
	*/
	RtcInterface::RtcInterface(const std::shared_ptr<Session>& s)
		: dtls_(RtcService::GetInstance().GetTaskQueueFactory())
		//, rtp_header_extension_map_() 
		, srtp_send_session_(nullptr)
		, srtp_recv_session_(nullptr)
		, rtp_header_()
		, extension_manager_()
		, twcc_context_()
		//, rtc_task_safety_(webrtc::PendingTaskSafetyFlag::Create())
		//, sctp_(nullptr)
		, session_(s)
	{
		
		local_ufrag_ = GetUFrag(8);
		int32_t count = 0;
		//防止突出的
		while (RtcService::GetInstance().FindUfragName(local_ufrag_) && ++count<=10)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "random local ufrag count = " << count;
			local_ufrag_ = GetUFrag(8);
		}
		local_passwd_ = GetUFrag(32);
		sdp_.SetLocalUFrag(local_ufrag_);
		sdp_.SetLocalPasswd(local_passwd_);
		extension_manager_.Register<libmedia_transfer_protocol::AbsoluteSendTime>(libmedia_transfer_protocol::kRtpExtensionAbsoluteSendTime);
		extension_manager_.Register<libmedia_transfer_protocol::TransportSequenceNumber>(libmedia_transfer_protocol::kRtpExtensionTransportSequenceNumber);
		// feedback  
		twcc_context_.setOnSendTwccCB([this](uint32_t ssrc, std::string fci) { onSendTwcc(ssrc, fci); });

		//sdp_.SetFingerprint(dtls_.Fingerprint());
		sdp_.SetLocalFingerprint(libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Fingerprints());
		// 本地ip port 
		sdp_.SetServerAddr(YamlConfig::GetInstance().GetRtcServerConfig().ips.at(0));
		sdp_.SetServerPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		sdp_.SetServerExternAddr(YamlConfig::GetInstance().GetRtcServerConfig().extern_ip);
		sdp_.SetServerExternPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);

		//CheckTimeOut();

	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC接口析构函数（RTC Interface Destructor）
	*  
	*  该析构函数用于清理RTC接口的资源。
	*  
	*  @note SRTP会话、DTLS对象等资源由派生类负责清理
	*/
	RtcInterface::~RtcInterface() {}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 生成ICE用户名片段（Get UFrag）
	*  
	*  该方法用于生成随机的ICE用户名片段，用于ICE连接检查的认证。
	*  
	*  生成流程：
	*  1. 定义字符表，包含数字、小写字母、大写字母
	*  2. 使用随机数生成器生成随机索引
	*  3. 从字符表中随机选择字符，组成指定长度的字符串
	*  
	*  @param size 生成的字符串长度
	*  @return 随机生成的ICE用户名片段
	*  @note ICE用户名片段长度通常为4-256个字符
	*  @note 使用静态随机数生成器，避免重复初始化
	*/
	std::string RtcInterface::GetUFrag(int size) {
		static std::string table = "1234567890abcdefgihjklmnopqrstuvwsyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::string frag;

		static std::mt19937 mt{ std::random_device{}() };
		static std::uniform_int_distribution<> rand(0, table.size());

		frag.resize(size);
		for (int i = 0; i < size; i++)
		{
			frag[i] = table[(rand(mt) % table.size())];
		}

		return frag;
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 生成SSRC（Get SSRC）
	*  
	*  该方法用于生成随机的SSRC（Synchronization Source Identifier），
	*  用于标识RTP流的源。
	*  
	*  生成流程：
	*  1. 使用随机数生成器生成10000000-99999999之间的随机数
	*  2. 返回生成的SSRC
	*  
	*  @param size 参数未使用，保留用于扩展
	*  @return 随机生成的SSRC
	*  @note SSRC必须唯一，避免与其他流冲突
	*  @note SSRC为32位无符号整数
	*/
	uint32_t RtcInterface::GetSsrc(int size)
	{
		static std::mt19937 mt{ std::random_device{}() };
		static std::uniform_int_distribution<> rand(10000000, 99999999);

		return rand(mt);
	}

	//void RtcInterface::SetStunTime()
	//{
	//	rtc_stun_timestamp_ = rtc::TimeMillis();
	//}

	//bool RtcInterface::CheckTimeOut()
	//{
	//	return (rtc::TimeMillis() - rtc_stun_timestamp_) >  YamlConfig::GetInstance().GetRtcServerConfig().timeout_ms;
	//	//std::weak_ptr<RtcInterface> weak_rtc = shared_from_this();
	//	//GbMediaService::GetInstance().worker_thread()->PostDelayedTask(ToQueuedTask(rtc_task_safety_,
	//	//	[this]() {
	//	//		 

	//	//		 
	//	//		if (rtc::TimeMillis() - rtc_stun_timestamp_ > (YamlConfig::GetInstance().GetRtcServerConfig().timeout_ms * 1)  && !destoy_)
	//	//		{

	//	//			// 删除当前rtc 
	//	//			GBMEDIASERVER_LOG(LS_WARNING) << " rtc time out !!! diff ms :"<< (rtc::TimeMillis() - rtc_stun_timestamp_) << ",   config ms : " << YamlConfig::GetInstance().GetRtcServerConfig().timeout_ms;
	//	//			//if (!GetDestory())
	//	//			{

	//	//				RemoveGlobalData();
	//	//				destoy_ = true;
	//	//			}
	//	//			//GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << session_name;
	//	//				//http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//	//				//	auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//	//				//	http_ctx->PostRequest(res);
	//	//				//	http_ctx->WriteComplete(conn);
	//	//				//	});

	//	//			//auto s = GbMediaService::GetInstance().CreateSession(session_name);
	//	//			//if (!s)
	//	//			//{
	//	//			//	GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << session_name;
	//	//			//	//http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//	//			//	//	auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//	//			//	//	http_ctx->PostRequest(res);
	//	//			//	//	http_ctx->WriteComplete(conn);
	//	//			//	//	});

	//	//			//	return;
	//	//			//}
	//	//			return;

	//	//		}
	//	//		if (destoy_)
	//	//		{
	//	//			return;
	//	//		}

	//	//		// 递归调用实现定时循环（每5秒）
	//	//		CheckTimeOut();
	//	//	}), 5000);
	//}
	bool RtcInterface::GetDestory()
	{
		return destoy_.load();
	}
	void RtcInterface::SetDeskory(bool value)
	{
		destoy_ = value;
	}
	libmedia_transfer_protocol::librtc::RtcSdpType RtcInterface::GetSdpType()
	{
		return sdp_.GetSdpType();
	}
	//void RtcInterface::RemoveGlobalData()
	//{
	//	if (destoy_)
	//	{
	//		return;
	//	}
	//	//libmedia_transfer_protocol::librtc::RtcSdpType rtc_sdp_type = sdp_.GetSdpType();
	//	//std::weak_ptr<RtcInterface> weak_rtc = shared_from_this();
	//	//GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [rtc_sdp_type, weak_rtc]() {

	//	//	auto rtc = weak_rtc.lock();
	//	//	if (!rtc)
	//	//	{
	//	//		return;
	//	//	}
	//	//	if (rtc->GetDestory())
	//	//	{
	//	//		return;
	//	//	}
	//	//	rtc->SetDeskory(true);

	//	//	std::string key = rtc->RtcRemoteAddress().ipaddr().ToString() + ":" + std::to_string(rtc->RtcRemoteAddress().port());
	//	//	std::string ufrag_name = rtc->LocalUFrag();// +":" + LocalPasswd();

	//	//	// 
	//	//	// global un
	//	////	RtcService::GetInstance().UnregisterRtcInterface(std::dynamic_pointer_cast<RtcInterface>(shared_from_this()));
	//	//	
	//	//	if (rtc_sdp_type == libmedia_transfer_protocol::librtc::kRtcSdpPlay)
	//	//	{
	//	//		
	//	//		//std::shared_ptr<RtcConsumer> consumer = std::dynamic_pointer_cast<RtcConsumer>(shared_from_this());
	//	//		 std::shared_ptr<RtcConsumer>  consumer = std::dynamic_pointer_cast<RtcConsumer>(rtc);// shared_from_this();
	//	//		 if (consumer)
	//	//		 {
	//	//			
	//	//			 GBMEDIASERVER_LOG(LS_WARNING) << " rtc time out !!! remove consumer stream_name = " << consumer->GetStream()->SessionName();
	//	//			 auto s = GbMediaService::GetInstance().CreateSession(consumer->GetSession()->GetStream()->SessionName());
	//	//			 if (s)
	//	//			 {
	//	//				 s->RemoveConsumer(consumer);
	//	//			 }
	//	//			 //auto session =  consumer->GetSession();// ->RemoveConsumer(weak_rtc);
	//	//			 //if (session)
	//	//			 //{
	//	//				// session->RemoveConsumer(weak_rtc);
	//	//			 //}
	//	//			// session_->RemoveConsumer(weak_rtc);
	//	//		 }
	//	//		
	//	//	}
	//	//	else if (rtc_sdp_type == libmedia_transfer_protocol::librtc::kRtcSdpPush)
	//	//	{
	//	//		std::shared_ptr<RtcProducer>  producer = std::dynamic_pointer_cast<RtcProducer>(rtc);// shared_from_this();
	//	//		if (producer)
	//	//		{
	//	//			GBMEDIASERVER_LOG(LS_WARNING) << " rtc time out !!! remove producer stream_name = " << producer->GetStream()->SessionName();
	//	//			auto s = GbMediaService::GetInstance().CreateSession(producer->GetStream()->SessionName());
	//	//			if (s)
	//	//			{
	//	//				s->SetProducer(nullptr);
	//	//			}
	//	//		}

	//	//		//session_->SetProducer(nullptr/*std::dynamic_pointer_cast<RtcProducer>(shared_from_this())*/);
	//	//	}
	//	//	else
	//	//	{
	//	//		GBMEDIASERVER_LOG(LS_WARNING) << " rtc time out !!! not find sdp type !!! stream_name = ";// << session_->GetStream()->SessionName();
	//	//	}

	//	//	RtcService::GetInstance().UnregisterRtcInterface(ufrag_name, key);

	//	//	//session_
	//	//	 //auto s = GbMediaService::GetInstance().CreateSession(session_name);
	//	//	//if (!s)
	//	//	//{
	//	//	//	GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << session_name;
	//	//		//http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//	//		//	auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//	//		//	http_ctx->PostRequest(res);
	//	//		//	http_ctx->WriteComplete(conn);
	//	//		//	});


	//	//	});
	//}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 设置RTC远程地址（Set RTC Remote Address）
	*  
	*  该方法用于设置远程客户端的地址，用于发送RTP/RTCP包。
	*  
	*  @param addr 远程客户端的Socket地址（IP地址和端口）
	*  @note 该地址在ICE连接建立后设置
	*  @note 所有RTP/RTCP包都会发送到该地址
	*/
	void  RtcInterface::SetRtcRemoteAddress(const rtc::SocketAddress& addr)
	{
		rtc_remote_address_ = addr;
	}


	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 发送SRTP RTP包（Send SRTP RTP）
	*  
	*  该方法用于加密RTP包并发送到远程客户端。
	*  
	*  处理流程：
	*  1. 检查DTLS握手是否完成，未完成则返回false
	*  2. 检查SRTP发送会话是否存在，不存在则返回false
	*  3. 使用SRTP发送会话加密RTP包
	*  4. 通过RTC服务器发送加密后的RTP包到远程地址
	*  5. 返回true表示发送成功
	*  
	*  SRTP加密：
	*  - SRTP（Secure Real-time Transport Protocol）是RTP的安全版本
	*  - SRTP使用AES加密算法加密RTP Payload
	*  - SRTP使用HMAC-SHA1算法生成认证标签
	*  - SRTP密钥在DTLS握手成功后导出
	*  
	*  @param data RTP数据包缓冲区（未加密）
	*  @param size 数据包大小
	*  @return 发送成功返回true，失败返回false
	*  @note 该方法会修改data指针和size值（加密后的数据）
	*  @note DTLS握手必须先完成，否则无法加密
	*/
	bool RtcInterface::SendSrtpRtp(uint8_t* data, size_t  size)
	{
		if (!dtls_done_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "dtls done failed !!!";
			return false;

		}
		if (!srtp_send_session_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "srtp_send_session_ == null failed !!!";
			return false;
		}
		//const uint8_t* data = single_packet->data();
		//size_t   len = single_packet->size();
		if (!srtp_send_session_->EncryptRtp((const uint8_t **)&data, &size))
		{
			return false;
		}


		GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, size),
			rtc_remote_address_, rtc::PacketOptions());


		return true;
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 发送SRTP RTCP包（Send SRTP RTCP）
	*  
	*  该方法用于加密RTCP包并发送到远程客户端。
	*  
	*  处理流程：
	*  1. 检查DTLS握手是否完成，未完成则返回false
	*  2. 检查SRTP发送会话是否存在，不存在则返回false
	*  3. 使用SRTP发送会话加密RTCP包
	*  4. 通过RTC服务器发送加密后的RTCP包到远程地址
	*  5. 返回true表示发送成功
	*  
	*  SRTCP加密：
	*  - SRTCP（Secure RTCP）是RTCP的安全版本
	*  - SRTCP使用与SRTP相同的加密算法和密钥
	*  - SRTCP加密RTCP包的内容，保护控制信息的安全
	*  
	*  @param data RTCP数据包缓冲区（未加密）
	*  @param size 数据包大小
	*  @return 发送成功返回true，失败返回false
	*  @note 该方法会修改data指针和size值（加密后的数据）
	*  @note DTLS握手必须先完成，否则无法加密
	*/
	bool RtcInterface::SendSrtpRtcp(uint8_t* data, size_t size)
	{

		if (!dtls_done_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "dtls done failed !!!";
			return false;

		}
		if (!srtp_send_session_)
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "srtp_send_session_ == null failed !!!";
			return false;
		}

#if 0
		// 检查并修复RTCP包头格式 (RFC 3550)
		// RTCP头第一个字节应该是 0x8X (版本2)
		// 如果是0x7X或其他错误格式，修正版本位

		if (size >= 1) {
			uint8_t first_byte = data[0];
			// 提取RC/FMT部分 (低5位)
			uint8_t rc_fmt = first_byte & 0x1F;
			// 提取版本位 (bit 6-7)
			uint8_t version = (first_byte >> 6) & 0x03;
			// 提取P位 (bit 5)
			uint8_t padding = (first_byte >> 5) & 0x01;
			
			// 如果版本不是2，修正它
			if (version != 2) {
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << "Fixing invalid RTCP version " 
					<< (int)version << " to 2, original byte: 0x"    << (int)first_byte/16;
				data[0] = 0x80 | (padding << 5) | rc_fmt;  // 设置版本=2, P=0
			}
		}
#endif // 
		if (!srtp_send_session_->EncryptRtcp((const uint8_t **)&data, &size))
		{
			return false;
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "hex:" << rtc::hex_encode((const char*)data, size);

		GbMediaService::GetInstance().GetRtcServer()->SendRtcpPacketTo(rtc::CopyOnWriteBuffer(data, size),
			rtc_remote_address_, rtc::PacketOptions());
		return true;
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 发送数据通道消息（Send Data Channel）
	*  
	*  该方法用于通过数据通道发送消息到远程客户端。
	*  
	*  处理流程：
	*  1. 检查SCTP关联是否存在
	*  2. 创建SCTP流参数，设置流ID
	*  3. 调用SCTP关联的SendSctpMessage方法发送消息
	*  
	*  数据通道说明：
	*  - WebRTC数据通道基于SCTP协议
	*  - 数据通道可以传输任意二进制数据或文本数据
	*  - 数据通道支持可靠传输和不可靠传输
	*  
	*  @param streamId SCTP流ID，标识数据通道
	*  @param ppid Payload Protocol Identifier，标识数据类型
	*  @param msg 消息数据缓冲区
	*  @param len 消息长度
	*  @note 该方法由上层应用调用，用于发送数据通道消息
	*  @note SCTP关联必须先建立，否则无法发送
	*/
	void RtcInterface::SendDatachannel(uint16_t streamId, uint32_t ppid, const char* msg, size_t len)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);

		if (sctp_) {
			libmedia_transfer_protocol::librtc::SctpStreamParameters params;
			params.streamId = streamId;
			sctp_->SendSctpMessage(params, ppid, (uint8_t*)msg, len);
		}
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 发送TWCC反馈（On Send TWCC）
	*  
	*  该方法是TWCC上下文的回调函数，用于发送TWCC反馈到远程客户端。
	*  
	*  处理流程：
	*  1. 创建RTCP TWCC反馈包
	*  2. 设置TWCC反馈包的SSRC和媒体SSRC
	*  3. 将TWCC FCI（Feedback Control Information）数据复制到反馈包中
	*  4. 调用SendSrtpRtcp方法发送TWCC反馈包
	*  
	*  TWCC反馈说明：
	*  - TWCC（Transport Wide Congestion Control）用于带宽估计和拥塞控制
	*  - TWCC反馈包包含接收到的RTP包的接收时间
	*  - 发送端根据TWCC反馈计算往返时延和丢包率，调整发送码率
	*  - TWCC反馈包的类型为RTCP RTPFB，子类型为15（Transport Feedback）
	*  
	*  TWCC FCI格式：
	*  - Base Sequence Number：基准序列号（16位）
	*  - Packet Status Count：包状态数量（16位）
	*  - Reference Time：参考时间（24位）
	*  - Feedback Packet Count：反馈包计数（8位）
	*  - Packet Chunks：包状态块（可变长度）
	*  - Receive Deltas：接收时间增量（可变长度）
	*  
	*  @param ssrc 媒体SSRC
	*  @param twcc_fci TWCC FCI数据
	*  @note 该方法由TWCC上下文在接收到足够的RTP包后自动调用
	*  @note TWCC反馈包通常每隔100-200ms发送一次
	*/
	void RtcInterface::onSendTwcc(uint32_t ssrc, const std::string& twcc_fci)
	{
		auto rtcp = libmedia_transfer_protocol::librtcp:: RtcpFB::create(libmedia_transfer_protocol::librtcp::RTPFBType::RTCP_RTPFB_TWCC , twcc_fci.data(), twcc_fci.size());
		rtcp->ssrc = htonl(0);
		rtcp->ssrc_media = htonl(ssrc);
		//GBMEDIASERVER_LOG(LS_INFO) << "send twcc -- packet size:" << rtcp->getSize();
		SendSrtpRtcp((uint8_t*)rtcp.get(), rtcp->getSize());


		//SendSrtpRtcp();

		//std::unique_ptr< libmedia_transfer_protocol::rtcp::Pli> pli = std::make_unique< libmedia_transfer_protocol::rtcp::Pli>();
		//pli->SetSenderSsrc(sdp_.VideoSsrc());
		//pli->SetMediaSsrc(sdp_.VideoSsrc());

		//libmedia_transfer_protocol::rtcp::CompoundPacket compound;               // Builds a compound RTCP packet with
		//compound.Append(std::move(pli));                  // a receiver report, report block
	   // compound.Append(&fir);                 // and fir message.
		//rtc::Buffer packet = compound.Build();
		//SendSrtpRtcp(packet.data(), packet.size());
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 创建数据通道（Create Data Channel）
	*  
	*  该方法用于创建SCTP关联，建立数据通道。
	*  
	*  处理流程：
	*  1. 创建SCTP关联对象，传入工作线程、回调接口、流数量、缓冲区大小等参数
	*  2. 调用SCTP关联的TransportConnected方法，通知传输层已连接
	*  
	*  SCTP关联参数：
	*  - 工作线程：用于处理SCTP消息的线程
	*  - 回调接口：SCTP关联的回调接口（this）
	*  - 输入流数量：128（最大支持128个输入流）
	*  - 输出流数量：128（最大支持128个输出流）
	*  - 缓冲区大小：262144字节（256KB）
	*  - 是否为服务端：true（服务端模式）
	*  
	*  @note 该方法通常在DTLS握手成功后调用
	*  @note SCTP关联建立后，可以创建多个数据通道
	*/
	void RtcInterface::CreateDataChannel()
	{
		sctp_ = std::make_shared<libmedia_transfer_protocol::librtc::SctpAssociationImp>(GbMediaService::GetInstance().worker_thread(), this, 128, 128, 262144, true);
		sctp_->TransportConnected();
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 添加视频包到缓存（Add Video Packet）
	*  
	*  该方法用于将视频RTP包添加到缓存中，用于NACK重传。
	*  
	*  处理流程：
	*  1. 将RTP包添加到缓存map中，以序列号为键
	*  2. 计算需要保留的最小序列号（当前序列号 - kMaxVideoPacketSize）
	*  3. 删除序列号小于最小序列号的RTP包，释放内存
	*  
	*  缓存策略：
	*  - 使用map存储RTP包，以序列号为键，方便快速查找
	*  - 最多缓存kMaxVideoPacketSize（1000）个RTP包
	*  - 当缓存超过限制时，删除最旧的包
	*  - 使用lower_bound查找需要删除的包，提高效率
	*  
	*  NACK重传：
	*  - 当客户端检测到丢包时，会发送NACK请求
	*  - 服务器根据NACK请求的序列号，从缓存中查找对应的RTP包
	*  - 如果找到，则重新发送该RTP包
	*  - 如果未找到（已被删除），则无法重传
	*  
	*  @param rtp_packet RTP包的共享指针
	*  @note 该方法在发送视频RTP包后调用
	*  @note 缓存大小限制为1000个包，约占用1-2MB内存
	*/
	void RtcInterface::AddVideoPacket(std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend> &&  rtp_packet)
	{
		uint32_t  seq_number = rtp_packet->SequenceNumber();
		rtp_video_packets_[seq_number] = rtp_packet;
		auto it = rtp_video_packets_.lower_bound(seq_number - kMaxVideoPacketSize);
		rtp_video_packets_.erase(rtp_video_packets_.begin(), it);
	}
#if 0
	void RtcInterface::AddVideoPacket(std::shared_ptr<libmedia_transfer_protocol::RtpPacketToSend> rtp_packet)
	{
		uint32_t  seq_number = rtp_packet->SequenceNumber();
		rtp_video_packets_[seq_number] = rtp_packet;
		auto it = rtp_video_packets_.lower_bound(seq_number - kMaxVideoPacketSize);
		rtp_video_packets_.erase(rtp_video_packets_.begin(), it);
	}
#endif // 
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理NACK请求（Request NACK）
	*  
	*  该方法用于处理客户端发送的NACK请求，重传丢失的RTP包。
	*  
	*  处理流程：
	*  1. 检查NACK请求的媒体SSRC是否匹配视频SSRC
	*  2. 遍历NACK请求中的所有丢失包序列号
	*  3. 从缓存中查找对应的原始RTP包
	*  4. 创建新的RTX包对象（深拷贝），避免修改缓存中的原始包
	*  5. 复制原始包的头部信息
	*  6. 设置RTX包的Payload Type、SSRC和序列号
	*  7. 构建RTX Payload：原始序列号（2字节，大端序）+ 原始Payload
	*  8. 调用SendSrtpRtp方法发送RTX包
	*  
	*  RTX（Retransmission）重传说明：
	*  - RTX使用单独的SSRC和Payload Type
	*  - RTX包的Payload Type通常为视频Payload Type + 1（如97）
	*  - RTX包的SSRC为视频SSRC + 1
	*  - RTX包的序列号独立递增，不影响原始RTP包的序列号
	*  - RTX包的Payload格式：[原始序列号(2字节)] + [原始Payload]
	*  - 符合RFC 4588标准
	*  
	*  NACK请求格式：
	*  - NACK请求是RTCP RTPFB包，子类型为1（Generic NACK）
	*  - NACK请求包含一个或多个丢失包的序列号
	*  - 每个NACK请求可以包含多个序列号，使用位图表示连续的丢失包
	*  
	*  重要修复说明：
	*  - 本方法已修复原有的严重bug：直接修改缓存中的原始RTP包
	*  - 现在使用深拷贝创建新的RTX包，确保原始包不被修改
	*  - 支持同一个包被多次NACK重传
	*  - RTX包格式符合RFC 4588标准
	*  
	*  @param nack NACK请求对象，包含丢失包的序列号列表
	*  @note 该方法在接收到NACK请求时调用
	*  @note 如果缓存中没有对应的RTP包，则无法重传
	*  @note RTX序列号独立递增，避免与原始RTP包冲突
	*  @note 原始包在缓存中保持不变，可以被多次重传
	*/
	void RtcInterface::RequestNack(const libmedia_transfer_protocol::rtcp::Nack& nack)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO) << "media_ssrc:" << nack.media_ssrc();
		if (nack.media_ssrc() != sdp_.VideoSsrc())
		{
			GBMEDIASERVER_LOG_T_F(LS_INFO) << "[media_ssrc:" << nack.media_ssrc() << ", != " << sdp_.VideoSsrc() << " ]";
			return;
		}

		for (const auto& packetid : nack.packet_ids())
		{
			//GBMEDIASERVER_LOG(LS_INFO) << "NACK request for seq:" << packetid
				//<< ", original_seq:" << original_seq
			//	<< ", rtx_seq:" << video_rtx_seq_;
				//<< ", original_pt:" << (int)original_packet->PayloadType()
				//<< ", original_ssrc:" << original_packet->Ssrc();
#if 1
			auto iter =  rtp_video_packets_.find(packetid);
			if (iter != rtp_video_packets_.end())
			{
				if (iter->second)
				{
					// 获取原始RTP包（不修改它）
					auto original_packet = iter->second;
					uint16_t original_seq = original_packet->SequenceNumber();
					
					GBMEDIASERVER_LOG(LS_INFO) << "NACK request for seq:" << packetid 
					                           << ", original_seq:" << original_seq
					                           << ", rtx_seq:" << video_rtx_seq_
					                           << ", original_pt:" << (int)original_packet->PayloadType()
					                           << ", original_ssrc:" << original_packet->Ssrc();
					
					// 创建新的RTX包对象（深拷贝）
					auto rtx_packet = std::make_shared<libmedia_transfer_protocol::RtpPacketToSend>(&extension_manager_);
					
					// 复制原始包的头部信息（不包括Payload）
					rtx_packet->CopyHeaderFrom(*original_packet);
					
					// 设置RTX包的头部字段
					rtx_packet->SetPayloadType(sdp_.GetVideoPayloadRtxType());
					rtx_packet->SetSsrc(sdp_.VideoRtxSsrc());
					rtx_packet->SetSequenceNumber(video_rtx_seq_++);
					
					// 构建RTX Payload：原始序列号（2字节，大端序）+ 原始Payload
					size_t original_payload_size = original_packet->payload_size();
					uint8_t* rtx_payload = rtx_packet->AllocatePayload(2 + original_payload_size);
					
					if (rtx_payload)
					{
						// 写入原始序列号（大端序，网络字节序）
						rtx_payload[0] = (original_seq >> 8) & 0xFF;
						rtx_payload[1] = original_seq & 0xFF;
						
						// 复制原始Payload数据
						if (original_payload_size > 0)
						{
							memcpy(rtx_payload + 2, 
							       original_packet->payload().data(), 
							       original_payload_size);
						}
						
						GBMEDIASERVER_LOG(LS_INFO) << "RTX packet created: rtx_seq:" << (video_rtx_seq_ - 1)
						                           << ", rtx_pt:" << (int)rtx_packet->PayloadType()
						                           << ", rtx_ssrc:" << rtx_packet->Ssrc()
						                           << ", rtx_payload_size:" << rtx_packet->payload_size()
						                           << ", original_seq_in_payload:" << original_seq;
						
						// 发送RTX包
						SendSrtpRtp((uint8_t*)rtx_packet->data(), rtx_packet->size());
					}
					else
					{
						GBMEDIASERVER_LOG(LS_ERROR) << "Failed to allocate RTX payload for seq:" << packetid;
					}
				}
			}
			else
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "NACK request for seq:" << packetid 
				                              << " but packet not found in cache";
			}
#endif 
		}

	}
}