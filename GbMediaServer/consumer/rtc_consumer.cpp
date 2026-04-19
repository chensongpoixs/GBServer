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
				   date:  2025-10-13

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
 
#include <random>
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

namespace gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC消费者构造函数（RTC Consumer Constructor）
	*  
	*  该构造函数用于创建RTC消费者实例，初始化WebRTC相关的组件，包括SDP、DTLS、SSRC等。
	*  
	*  初始化流程：
	*  1. 调用基类RtcInterface和Consumer的构造函数
	*  2. 生成音频和视频的SSRC（Synchronization Source Identifier）
	*  3. 设置SDP中的音频SSRC、视频SSRC和视频RTX SSRC
	*  4. 连接DTLS信号槽，用于处理DTLS握手事件
	*  5. 设置SDP的本地DTLS指纹，用于DTLS握手验证
	*  6. 设置SDP的服务器地址和端口，从配置文件中读取
	*  7. 设置流名称，用于标识媒体流
	*  
	*  SSRC生成规则：
	*  - 音频SSRC：随机生成10位数字
	*  - 视频SSRC：音频SSRC + 1
	*  - 视频RTX SSRC：视频SSRC + 1（用于重传）
	*  
	*  DTLS信号槽连接：
	*  - SignalDtlsConnecting：DTLS连接中回调
	*  - SignalDtlsConnected：DTLS连接成功回调
	*  - SignalDtlsClose：DTLS连接关闭回调
	*  - SignalDtlsFailed：DTLS连接失败回调
	*  - SignalDtlsSendPakcet：DTLS发送数据包回调
	*  - SignalDtlsApplicationDataReceived：DTLS应用数据接收回调
	*  
	*  @param stream 流对象的共享指针，用于获取媒体流
	*  @param s 会话对象的共享指针，用于管理会话状态
	*  @note SSRC必须唯一，避免与其他流冲突
	*  @note DTLS指纹在程序启动时生成，所有连接共享同一个证书
	*/
	RtcConsumer::RtcConsumer(     std::shared_ptr<Stream> &stream, const  std::shared_ptr<Session> &s)
	: RtcInterface (s)
	, Consumer(stream, s)
	, task_safety_(webrtc::PendingTaskSafetyFlag::Create())
	, current_target_bitrate_bps_(0)
	//, outbound_loss_sim_(YamlConfig::GetInstance().GetRtcConsumerOutboundLossTestConfig())
	{
	
		
		uint32_t audio_ssrc = RtcInterface::GetSsrc(10);
		uint32_t video_ssrc = audio_ssrc + 1;

		
		sdp_.SetAudioSsrc(audio_ssrc);
		sdp_.SetVideoSsrc(video_ssrc);
		sdp_.SetVideoRtxSsrc(video_ssrc + 1);
		//dtls_certs_.Init();
		//dtls_.Init();
		//dtls_.SignalDtlsSendPakcet.connect(this, &PlayRtcUser::OnDtlsSendPakcet);
		//dtls_.SignalDtlsHandshakeDone.connect(this, &PlayRtcUser::OnDtlsHandshakeDone);
		//dtls_.SignalDtlsClose.connect(this, &PlayRtcUser::OnDtlsClosed);

		 
		dtls_.SignalDtlsConnecting.connect(this, &RtcConsumer::OnDtlsConnecting);
		dtls_.SignalDtlsConnected.connect(this, &RtcConsumer::OnDtlsConnected);
		dtls_.SignalDtlsClose.connect(this, &RtcConsumer::OnDtlsClosed);
		dtls_.SignalDtlsFailed.connect(this, &RtcConsumer::OnDtlsFailed);
		dtls_.SignalDtlsSendPakcet.connect(this, &RtcConsumer::OnDtlsSendPakcet);
		dtls_.SignalDtlsApplicationDataReceived.connect(this, &RtcConsumer::OnDtlsApplicationDataReceived);
		






		////sdp_.SetFingerprint(dtls_.Fingerprint());
		//sdp_.SetLocalFingerprint(libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Fingerprints());
		//// 本地ip port 
		//sdp_.SetServerAddr(YamlConfig::GetInstance().GetRtcServerConfig().ips.at(0));
		//sdp_.SetServerPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		//sdp_.SetServerExternAddr(YamlConfig::GetInstance().GetRtcServerConfig().extern_ip);
		//sdp_.SetServerExternPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		//sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);


		// 创建统计对象并注册到统计管理器（Create statistics object and register to manager）
		// @date 2025-10-18
		std::string consumer_id = s->SessionName() + "_consumer_" + std::to_string(audio_ssrc);
		statistics_ = std::make_shared<ConsumerStatistics>(
			consumer_id,
			s->SessionName(),
			s->SessionName(),
			"rtc"
		);
		StatisticsManager::GetInstance().RegisterConsumer(consumer_id, statistics_);
		statistics_->SetState("created");

		// 设置媒体信息（Set media info）
		// @date 2025-10-18
		statistics_->SetVideoInfo(
			video_ssrc,
			video_ssrc + 1, // RTX SSRC
			0, // width未知
			0, // height未知
			"H264"
		);
		statistics_->SetAudioInfo(audio_ssrc, "OPUS");

		/*if (YamlConfig::GetInstance().GetRtcConsumerOutboundLossTestConfig().enabled) {
			GBMEDIASERVER_LOG(LS_WARNING)
			    << "RtcConsumer outbound RTP loss simulation ENABLED (test only), session="
			    << s->SessionName();
		}*/
	
		//rtp_header_extension_map_.Register<libmedia_transfer_protocol::TransportSequenceNumber>(libmedia_transfer_protocol::kRtpExtensionTransportSequenceNumber);
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC消费者析构函数（RTC Consumer Destructor）
	*  
	*  该析构函数用于清理RTC消费者实例，释放所有相关资源。
	*  
	*  清理流程：
	*  1. 记录析构日志
	*  2. 删除SRTP发送会话，释放加密资源
	*  3. 删除SRTP接收会话，释放解密资源
	*  4. 设置dtls_done_标志为false，表示DTLS连接已关闭
	*  5. 删除Muxer对象，释放媒体复用资源
	*  6. 从SRTP会话中移除视频和音频流
	*  7. 断开所有DTLS信号槽连接
	*  
	*  资源清理顺序：
	*  - 先清理SRTP会话（依赖DTLS密钥）
	*  - 再清理Muxer对象（依赖媒体流）
	*  - 最后断开DTLS信号槽（避免回调到已销毁的对象）
	*  
	*  @note 必须先删除SRTP会话，再断开DTLS信号槽
	*  @note 流对象和会话对象由智能指针管理，会自动释放
	*  @note 注意避免重复删除SRTP会话（代码中有两次删除srtp_send_session_）
	*/
	RtcConsumer:: ~RtcConsumer(){
		GBMEDIASERVER_LOG_T_F(LS_INFO);

		// 从统计管理器注销（Unregister from statistics manager）
		// @date 2025-10-18
		if (statistics_) {
			StatisticsManager::GetInstance().UnregisterConsumer(statistics_->GetConsumerId());
		}
		
		//dtls_.SignalDtlsSendPakcet.disconnect(this);
		//dtls_.SignalDtlsHandshakeDone.disconnect(this);
		//dtls_.SignalDtlsClose.disconnect(this);

		if (srtp_send_session_)
		{
			delete srtp_send_session_;
			srtp_send_session_ = nullptr;
		}
		if (srtp_recv_session_)
		{
			delete  srtp_recv_session_;
			srtp_recv_session_ = nullptr;
		}

		dtls_done_ = false;
		if (muxer_)
		{
			delete muxer_;
			muxer_ = nullptr;
		}
		if (srtp_send_session_)
		{
			srtp_send_session_->RemoveStream(sdp_.VideoSsrc());
			srtp_send_session_->RemoveStream(sdp_.AudioSsrc());
		}
		dtls_.disconnect_all();
		
		

		
		
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 请求关键帧（Request Key Frame）
	*  
	*  该方法用于向生产者请求发送关键帧（IDR帧）。当客户端解码器需要重新同步
	*  或网络恢复后需要快速恢复播放时，会调用该方法请求关键帧。
	*  
	*  处理流程：
	*  1. 调用Session的ConsumerRequestKeyFrame方法
	*  2. Session通知Producer生成关键帧
	*  3. Producer生成关键帧后，通过Stream分发给所有Consumer
	*  
	*  @note 该方法通常由RTCP PLI或FIR反馈触发
	*  @note 频繁请求关键帧会增加带宽消耗
	*/
	void RtcConsumer::RequestKeyFrame()
	{
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE,
			[this]() {
				if (GetSession())
				{
					GetSession()->ConsumerRequestKeyFrame();
				}
				
		});
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理Offer SDP（Process Offer SDP）
	*  
	*  该方法用于处理客户端发送的SDP Offer，解析媒体能力、ICE候选、DTLS指纹等信息。
	*  
	*  处理流程：
	*  1. 设置SDP类型为Offer
	*  2. 调用SDP对象的Decode方法解析SDP字符串
	*  3. 提取音视频编解码器、ICE候选、DTLS指纹等信息
	*  
	*  @param rtc_sdp_type SDP类型（Offer或Answer）
	*  @param sdp SDP字符串，包含完整的SDP描述
	*  @return 解析成功返回true，失败返回false
	*  @note 该方法必须在DTLS握手前调用
	*/
	bool RtcConsumer::ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string &sdp) {
		sdp_.SetSdpType(rtc_sdp_type);
		return sdp_.Decode(sdp);
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 获取本地ICE用户名片段（Local UFrag）
	*  
	*  @return 本地ICE用户名片段
	*/
	const std::string &RtcConsumer::LocalUFrag() const {
		return sdp_.GetLocalUFrag();
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 获取本地ICE密码（Local Password）
	*  
	*  @return 本地ICE密码
	*/
	const std::string &RtcConsumer::LocalPasswd() const {
		return sdp_.GetLocalPasswd();
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 获取远程ICE用户名片段（Remote UFrag）
	*  
	*  @return 远程ICE用户名片段
	*/
	const std::string &RtcConsumer::RemoteUFrag() const {
		return sdp_.GetRemoteUFrag();
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 构建Answer SDP（Build Answer SDP）
	*  
	*  该方法用于生成本地的SDP Answer，包含本地的媒体能力、ICE候选、DTLS指纹等信息。
	*  
	*  @return SDP Answer字符串
	*  @note 该方法必须在ProcessOfferSdp之后调用
	*/
	std::string RtcConsumer::BuildAnswerSdp() {
		//sdp_.SetFingerprint(dtls_.Fingerprint());
		return sdp_.Encode();
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 启动DTLS握手（May Run DTLS）
	*  
	*  该方法用于启动DTLS握手，根据SDP中的setup属性决定是作为客户端还是服务端。
	*  
	*  处理流程：
	*  1. 设置DTLS的远程指纹，用于验证对端证书
	*  2. 从SDP中获取远程角色（active、passive、actpass）
	*  3. 根据远程角色决定本地角色：
	*     - 远程为actpass或active：本地作为服务端（SERVER）
	*     - 远程为passive：本地作为客户端（CLIENT），主动发起握手
	*  4. 记录角色信息到日志
	*  5. 调用DTLS的Run方法，启动握手
	*  
	*  DTLS角色说明：
	*  - active：主动发起握手的一方（客户端）
	*  - passive：被动等待握手的一方（服务端）
	*  - actpass：可以作为客户端或服务端，由对端决定
	*  - holdconn：暂不建立连接
	*  
	*  角色协商规则：
	*  - 如果一方是actpass，另一方可以选择active或passive
	*  - 如果一方是active，另一方必须是passive
	*  - 如果一方是passive，另一方必须是active
	*  
	*  @note 该方法通常在ICE连接建立后调用
	*  @note DTLS握手成功后会触发OnDtlsConnected回调
	*/
	void RtcConsumer::MayRunDtls()
	{
		dtls_.SetRemoteFingerprint(sdp_.GetRemoteFingerprint());

		// client and server 
		std::string  role = sdp_.GetRemoteRole();
		 //// role = "active" / "passive" / "actpass" / "holdconn"
		/*a = setup 主要是表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器
		a = setup:actpass 既可以是客户端，也可以是服务器
		a = setup : active 客户端
		a = setup : passive 服务器
		由客户端先发起client hello*/
		libmedia_transfer_protocol::libssl::Role   local_role = libmedia_transfer_protocol::libssl::Role::SERVER;
		if (role == "actpass" || role == "active")
		{
			local_role = libmedia_transfer_protocol::libssl::Role::SERVER;
		}
		else if (role == "passive")
		{
			//远端是服务时候就需要自动发送Hello 
			local_role = libmedia_transfer_protocol::libssl::Role::CLIENT;
		}
		GBMEDIASERVER_LOG(LS_INFO) << "remote role:"<< role <<" , local_role :" << (int32_t)local_role;
		dtls_.Run(local_role);
	}

#if 0
	  std::string RtcConsumer::GetUFrag(int size) {
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
	  uint32_t RtcConsumer::GetSsrc(int size)
	  {
		  static std::mt19937 mt{ std::random_device{}() };
		  static std::uniform_int_distribution<> rand(10000000, 99999999);

		  return rand(mt);
	  }
#endif // 

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理接收到的DTLS数据（On DTLS Receive）
	*  
	*  该方法用于处理从网络接收到的DTLS数据包，将其传递给DTLS对象进行处理。
	*  
	*  @param buf DTLS数据包缓冲区
	*  @param size 数据包大小
	*  @note 该方法由RtcService在接收到DTLS包时调用
	*/
	  void RtcConsumer::OnDtlsRecv(const uint8_t *buf, size_t size)
	  {
		  dtls_.OnRecv(buf, size);
	  }

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理SRTP RTP包（On SRTP RTP）
	*  
	*  该方法用于处理从客户端接收到的SRTP RTP包，解密后提取媒体数据。
	*  当前实现为空，因为RTC消费者只发送RTP包，不接收RTP包。
	*  
	*  @param data RTP数据包缓冲区（已解密）
	*  @param size 数据包大小
	*  @note 该方法由RtcService在接收到RTP包时调用
	*  @note RTC消费者通常不需要接收RTP包，该方法为空实现
	*/
	  void RtcConsumer::OnSrtpRtp(  uint8_t * data, size_t size)
	  {
		  GBMEDIASERVER_LOG_T_F(LS_INFO) << "";
	  }
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理SRTP RTCP包（On SRTP RTCP）
	*  
	*  该方法用于处理从客户端接收到的SRTP RTCP包，解密后解析控制信息并进行相应处理。
	*  
	*  处理流程：
	*  1. 使用SRTP接收会话解密RTCP包
	*  2. 解析RTCP复合包，可能包含多个RTCP块
	*  3. 遍历所有RTCP块，根据类型进行不同处理：
	*     - SR（Sender Report）：发送者报告，记录发送统计信息
	*     - RR（Receiver Report）：接收者报告，记录接收统计信息
	*     - SDES（Source Description）：源描述信息
	*     - BYE：会话结束通知
	*     - APP：应用定义的RTCP包
	*     - RTPFB（RTP Feedback）：RTP反馈，包含NACK重传请求、TMMBR等
	*     - PSFB（Payload-Specific Feedback）：负载特定反馈，包含PLI/FIR关键帧请求
	*  4. 根据反馈类型执行相应操作（重传、关键帧请求等）
	*  
	*  RTCP包类型处理：
	*  - SR（200）：记录发送者的发送统计信息
	*  - RR（201）：记录接收者的接收统计信息
	*  - SDES（202）：源描述信息
	*  - BYE（203）：会话结束
	*  - APP（204）：应用定义
	*  - RTPFB（205）：RTP反馈
	*    - NACK（1）：请求重传丢失的RTP包
	*    - TMMBR（3）：临时最大媒体流比特率请求
	*    - TMMBN（4）：临时最大媒体流比特率通知
	*    - RapidResyncRequest（5）：快速重新同步请求
	*    - TransportFeedback（15）：传输层反馈，用于带宽估计
	*  - PSFB（206）：负载特定反馈
	*    - PLI（1）：图像丢失指示，请求关键帧
	*    - FIR（4）：完整帧内请求，请求关键帧
	*    - AFB（15）：应用层反馈块
	*  
	*  @param data RTCP数据包缓冲区（已加密）
	*  @param size 数据包大小
	*  @note 该方法由RtcService在接收到RTCP包时调用
	*  @note RTCP包必须先通过SRTP解密才能处理
	*  @note PLI和FIR反馈会触发关键帧请求
	*  @note NACK反馈会触发RTP包重传
	*/
	  void RtcConsumer::OnSrtpRtcp(  uint8_t * data, size_t size)
	  {
		  //GBMEDIASERVER_LOG_T_F(LS_INFO);
		  if (!srtp_recv_session_->DecryptSrtcp((uint8_t*)data, (size_t*)&size))
		  {
			  GBMEDIASERVER_LOG_T_F(LS_WARNING) << "decrypt srtcp failed !!!";
			  return;
		  }
 
		  rtc::ArrayView<const uint8_t>  packet(data, size);
		  libmedia_transfer_protocol::rtcp::CommonHeader rtcp_block;
		  // If a sender report is received but no DLRR, we need to reset the
	// roundTripTime stat according to the standard, see
	// https://www.w3.org/TR/webrtc-stats/#dom-rtcremoteoutboundrtpstreamstats-roundtriptime
		  struct RtcpReceivedBlock {
			  bool sender_report = false;
			  bool dlrr = false;
		  };
		  // For each remote SSRC we store if we've received a sender report or a DLRR
		  // block.
		  webrtc::flat_map<uint32_t, RtcpReceivedBlock> received_blocks;
		  for (const uint8_t* next_block = packet.begin(); next_block != packet.end();
			  next_block = rtcp_block.NextPacket()) {
			  ptrdiff_t remaining_blocks_size = packet.end() - next_block;
			  RTC_DCHECK_GT(remaining_blocks_size, 0);
			  if (!rtcp_block.Parse(next_block, remaining_blocks_size)) {
				  if (next_block == packet.begin()) {
					  // Failed to parse 1st header, nothing was extracted from this packet.
					  RTC_LOG(LS_WARNING) << "Incoming invalid RTCP packet";
					  return;
				  }
				//  ++num_skipped_packets_;
				  break;
			  }

			  //if (packet_type_counter_.first_packet_time_ms == -1)
			  //	packet_type_counter_.first_packet_time_ms = clock_->TimeInMilliseconds();
			  //RTC_LOG_F(LS_INFO) << "recvice RTCP TYPE = " << rtcp_block.type();
			  switch (rtcp_block.type()) {
			  case libmedia_transfer_protocol::rtcp::SenderReport::kPacketType:
			  {
				 // RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type();
				  //HandleSenderReport(rtcp_block, packet_information);
				  // 
				  //received_blocks[packet_information->remote_ssrc].sender_report = true;
				  libmedia_transfer_protocol::rtcp::SenderReport sender_report;
				  if (!sender_report.Parse(rtcp_block)) {
					 // ++num_skipped_packets_;
					  return;
				  }

				  RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type()
					  << ", ssrc:" << sender_report.sender_ssrc();
				/*  if (rtcp_context_recv_ && sender_report.sender_ssrc() == sdp_.VideoSsrc())
				  {
					  rtcp_context_recv_->onRtcp(&sender_report);
				  }*/
				  break;
			  }
			  case libmedia_transfer_protocol::rtcp::ReceiverReport::kPacketType:
				  RTC_LOG_F(LS_INFO) << "recvice RR RTCP TYPE = " << rtcp_block.type();
				  //HandleReceiverReport(rtcp_block, packet_information);
				  {
					  libmedia_transfer_protocol::rtcp::ReceiverReport rr;
					  if (rr.Parse(rtcp_block)) {
						  // 遍历所有Report Block
						  for (const auto& block : rr.report_blocks()) {
							  if (block.source_ssrc() == sdp_.VideoSsrc()) {
								  // 更新丢包信息
								  int64_t packets_lost = block.cumulative_lost();
								  int64_t packets_expected = block.extended_high_seq_num() - video_seq_ + 1;
								  
								  bandwidth_estimation_.UpdatePacketsLost(packets_lost, packets_expected);
								  
								  // 更新RTT（如果有DLSR信息）
								  if (block.last_sr() > 0 && block.delay_since_last_sr() > 0) {
									  uint32_t delay_ms = (block.delay_since_last_sr() * 1000) / 65536;
									  bandwidth_estimation_.UpdateRtt(static_cast<int32_t>(delay_ms));
								  }
								  
								  GBMEDIASERVER_LOG(LS_INFO) << "RR received: lost=" << packets_lost 
															  << ", fraction=" << (int)block.fraction_lost()
															  << ", jitter=" << block.jitter();
							  }
						  }
					  }
				  }
				  break;
			  case libmedia_transfer_protocol::rtcp::Sdes::kPacketType:
				  RTC_LOG(LS_INFO) << "recvice SDES RTCP TYPE = " << rtcp_block.type();
				  //HandleSdes(rtcp_block, packet_information);
				  break;
			  case libmedia_transfer_protocol::rtcp::ExtendedReports::kPacketType: {
				  RTC_LOG(LS_INFO) << "recvice ExtenderR RTCP TYPE = " << rtcp_block.type();
				  //bool contains_dlrr = false;
				  //uint32_t ssrc = 0;
				  //HandleXr(rtcp_block, packet_information, contains_dlrr, ssrc);
				  //if (contains_dlrr) {
				  //	received_blocks[ssrc].dlrr = true;
				  //}
				  break;
			  }
			  case libmedia_transfer_protocol::rtcp::Bye::kPacketType:
				  RTC_LOG(LS_INFO) << "recvice Bye RTCP TYPE = " << rtcp_block.type();
				  //HandleBye(rtcp_block);
				  break;
			  case libmedia_transfer_protocol::rtcp::App::kPacketType:
				  RTC_LOG(LS_INFO) << "recvice App RTCP TYPE = " << rtcp_block.type();
				  //HandleApp(rtcp_block, packet_information);
				  break;
			  case libmedia_transfer_protocol::rtcp::Rtpfb::kPacketType:
				  //RTC_LOG_F(LS_INFO) << "recvice rtpfb ";
				  switch (rtcp_block.fmt()) {
					  case libmedia_transfer_protocol::rtcp::Nack::kFeedbackMessageType:
					  {
						  RTC_LOG_F(LS_INFO) << "recvice rtpfb  nack RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						  //HandleNack(rtcp_block, packet_information);
						  libmedia_transfer_protocol::rtcp::Nack nack;
						  if (!nack.Parse(rtcp_block)) 
						  {
							  GBMEDIASERVER_LOG_T_F(LS_WARNING) << "parse recvice rtpfb nack failed !!!";
							 // ++num_skipped_packets_;
							  continue;
						  }

						  // 统计NACK发送（Statistics NACK sent）
						  // @date 2025-10-18
						  if (statistics_) {
							  statistics_->OnNackSent(nack.packet_ids().size());
						  }
						 
						RequestNack(nack);
							  
						  break;
					  }
					  case libmedia_transfer_protocol::rtcp::Tmmbr::kFeedbackMessageType:
				  		RTC_LOG(LS_INFO) << "recvice rtpfb  tmmbr RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleTmmbr(rtcp_block, packet_information);
				  		break;
					  case libmedia_transfer_protocol::rtcp::Tmmbn::kFeedbackMessageType:
				  		RTC_LOG(LS_INFO) << "recvice rtpfb tmmbn RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleTmmbn(rtcp_block, packet_information);
				  		break;
					  case libmedia_transfer_protocol::rtcp::RapidResyncRequest::kFeedbackMessageType:
				  		RTC_LOG(LS_INFO) << "recvice rtpfb rapidresy ync  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleSrReq(rtcp_block, packet_information);
				  		break;
					  case libmedia_transfer_protocol::rtcp::TransportFeedback::kFeedbackMessageType:
				  		 // RTC_LOG_F(LS_INFO) << "recvice rtpfb transport feedback  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				  		//HandleTransportFeedback(rtcp_block, packet_information);
				  		break;
					  default:
				  		//++num_skipped_packets_;
				  		break;
				  }
				  break;
			  case libmedia_transfer_protocol::rtcp::Psfb::kPacketType:
				 // RTC_LOG(LS_INFO) << "recvice psfb  pli";
				   switch (rtcp_block.fmt()) {
					  case libmedia_transfer_protocol::rtcp::Pli::kFeedbackMessageType:
						  RTC_LOG(LS_INFO) << "recvice psfb  pli  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						 // HandlePli(rtcp_block, packet_information);

						  // 统计PLI发送（Statistics PLI sent）
						  // @date 2025-10-18
						  if (statistics_) {
							  statistics_->OnPliSent();
						  }

						  RequestKeyFrame();
						  break;
					  case libmedia_transfer_protocol::rtcp::Fir::kFeedbackMessageType:
						  RTC_LOG(LS_INFO) << "recvice psfb  fir  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						  //HandleFir(rtcp_block, packet_information);

						  // 统计FIR发送（Statistics FIR sent）
						  // @date 2025-10-18
						  if (statistics_) {
							  statistics_->OnFirSent();
						  }

						  RequestKeyFrame();
						  break;
					  case libmedia_transfer_protocol::rtcp::Psfb::kAfbMessageType:
						  RTC_LOG(LS_INFO) << "recvice psfb  psfb  af  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
						  //HandlePsfbApp(rtcp_block, packet_information);
						  break;
					  default:
						 // ++num_skipped_packets_;
						  break;
				   } 
				  break;
			  default:
				 // ++num_skipped_packets_;
				  break;
			  }
		  }
	  }
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理视频帧（On Video Frame）
	*  
	*  该方法用于处理从Stream接收到的视频帧，将其封装为RTP包并通过SRTP发送。
	*  
	*  处理流程：
	*  1. 检查DTLS握手是否完成，未完成则直接返回
	*  2. 检查视频帧大小是否有效（至少5字节）
	*  3. 计算RTP时间戳（视频帧时间戳 * 90，90kHz时钟频率）
	*  4. 检查SRTP发送会话是否存在
	*  5. 创建RTP打包器，根据H264 NALU大小决定打包策略：
	*     - 单NALU模式：NALU小于MTU，直接封装为一个RTP包
	*     - FU-A分片模式：NALU大于MTU，分片为多个RTP包
	*  6. 遍历所有RTP包，设置RTP头部信息：
	*     - Payload Type：视频负载类型（通常为96-127）
	*     - Timestamp：RTP时间戳
	*     - SSRC：视频流的同步源标识符
	*     - Sequence Number：RTP序列号（递增）
	*  7. 添加RTP扩展（如传输序列号）
	*  8. 调用SendSrtpRtp方法，使用SRTP加密并发送RTP包
	*  9. 将RTP包添加到视频包缓存，用于NACK重传
	*  
	*  RTP打包策略：
	*  - H264 NALU单元会根据大小进行不同的打包
	*  - 小NALU（< MTU）：单NALU模式，一个RTP包包含一个NALU
	*  - 大NALU（>= MTU）：FU-A分片模式，一个NALU分片为多个RTP包
	*  - 多个小NALU：STAP-A聚合模式（当前未使用）
	*  
	*  RTP时间戳计算：
	*  - 视频帧时间戳单位通常为微秒（μs）
	*  - RTP时间戳单位为90kHz时钟周期
	*  - 转换公式：RTP时间戳 = 视频帧时间戳 * 90 / 1000
	*  
	*  @param frame 编码后的视频帧，包含H264编码数据和元信息
	*  @note 该方法会在Stream分发视频帧时被调用
	*  @note 视频帧会被分片为多个RTP包，每个包不超过MTU大小
	*  @note RTP包会通过SRTP加密后发送
	*  @note 注释掉的代码（#if 0）是旧的发送方式，已被SendSrtpRtp替代
	*/
	  void RtcConsumer::OnVideoFrame(const libmedia_codec::EncodedImage &frame)
	  {
		  if (!dtls_done_)
		  {
			  return;
		  }
		  //if (dtls_.)

		  if (frame.size() < 5)
		  {
			  return;
		  }
		  
		  uint32_t rtp_timestamp = frame.Timestamp() * 90;
		  
		  if (!srtp_send_session_)
		  {
			  GBMEDIASERVER_LOG(LS_WARNING) << "ignoring RTP packet due to non sending SRTP session";
			  return;
		  }

		  // 更新SR统计
		  sr_rtp_timestamp_ = rtp_timestamp;
		  sr_packet_count_ += 1;
		  sr_octet_count_ += static_cast<uint32_t>(frame.size());

		  libmedia_transfer_protocol::RtpPacketizer::PayloadSizeLimits   limits;
		  libmedia_transfer_protocol::RTPVideoHeader   rtp_video_hreader;
		 
		  webrtc::RTPVideoHeaderH264  h;
		  // 多包和分包
		  h.packetization_mode = webrtc::H264PacketizationMode::NonInterleaved;
		  rtp_video_hreader.video_type_header = h;
		  absl::optional<libmedia_codec::VideoCodecType> video_type = libmedia_codec::kVideoCodecH264;
		  // for (int32_t nal = 0; nal < fragments_count; ++nal)
		  {
			  std::unique_ptr<libmedia_transfer_protocol::RtpPacketizer> packetizer = 
				  libmedia_transfer_protocol::RtpPacketizer::Create(
				  video_type, rtc::ArrayView<const uint8_t>(frame.data() /*+nalus[nal].payload_start_offset */, frame.size()
					   /*nalus[nal].payload_size*/ ),
				  limits, rtp_video_hreader);

			  // std::vector< std::unique_ptr<libmedia_transfer_protocol::RtpPacketToSend>>  packets;

			  int32_t  number_packet = packetizer->NumPackets();
			  for (int32_t i = 0; i < number_packet; ++i)
			  {

				  auto  single_packet =
					  std::make_shared<libmedia_transfer_protocol::RtpPacketToSend>(&extension_manager_);

				  single_packet->SetPayloadType(sdp_.GetVideoPayloadType());
				  single_packet->SetTimestamp(rtp_timestamp);
				  single_packet->SetSsrc(sdp_.VideoSsrc());
				  single_packet->ReserveExtension<libmedia_transfer_protocol::TransportSequenceNumber>();

				  if (!packetizer->NextPacket(single_packet.get()))
				  {
					  break;
				  }
				  single_packet->SetSequenceNumber(video_seq_++);
				  single_packet->set_packet_type(libmedia_transfer_protocol::RtpPacketMediaType::kVideo);
				  single_packet->SetExtension<libmedia_transfer_protocol::TransportSequenceNumber>(
					  NextOutboundTransportSequenceNumber());


#if  0
				  const uint8_t *data = single_packet->data();
				  size_t   len = single_packet->size();
				  if (!srtp_send_session_->EncryptRtp(&data, &len))
				  {
					  continue;
				  }
				  

				  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), rtc_remote_address_, rtc::PacketOptions());
				  //packets.push_back( std::move(single_packet));
#else
				  // 出站丢包模拟：丢弃时不 SendSrtpRtp，仍 AddVideoPacket 以便 NACK/RTX 测试闭环
				  //if (!outbound_loss_sim_.ShouldDropOutboundRtp()) 
				 /* if (current_time_packet_ > std::time(NULL))
				  {*/
					  SendSrtpRtp((uint8_t*)single_packet->data(), single_packet->size());
					  if (statistics_) {
						  statistics_->OnRtpPacketSent(true, single_packet->size());
					  }
				  /*}
				  else
				  {
					  current_time_packet_ = std::time(NULL) + 3;
				  }*/

#endif  
				  AddVideoPacket(std::move(single_packet));
			  }
		  }

		  // 统计视频帧发送（Statistics video frame sent）
		  // @date 2025-10-18
		  if (statistics_) {
			  // 简单判断是否为关键帧（通过检查NALU类型）
			  bool is_key_frame = false;
			  if (frame.size() > 4) {
				  uint8_t nal_type = frame.data()[4] & 0x1F;
				  is_key_frame = (nal_type == 5); // IDR帧
			  }
			  statistics_->OnFrameSent(true, is_key_frame);
		  }
	  }
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理音频帧（On Audio Frame）
	*  
	*  该方法用于处理从Stream接收到的音频帧，将其封装为RTP包并通过SRTP发送。
	*  
	*  处理流程：
	*  1. 注释掉的代码用于AAC转OPUS，暂时不支持，后期可能支持
	*  2. 创建RTP包，设置RTP头部信息：
	*     - Payload Type：音频负载类型（通常为96-127）
	*     - Timestamp：音频帧的PTS时间戳
	*     - SSRC：音频流的同步源标识符
	*     - Marker：设置为true，表示音频帧边界
	*     - Sequence Number：RTP序列号（递增）
	*  3. 添加RTP扩展（如传输序列号）
	*  4. 分配RTP Payload空间，复制音频编码数据
	*  5. 调用SendSrtpRtp方法，使用SRTP加密并发送RTP包
	*  
	*  音频RTP打包：
	*  - 音频帧通常较小，一个RTP包可以包含一个音频帧
	*  - 音频编码格式可能为OPUS、PCMU、PCMA等
	*  - 音频时间戳直接使用PTS，不需要转换
	*  - Marker位设置为true，表示音频帧的结束
	*  
	*  @param frame 音频编码数据缓冲区
	*  @param pts 呈现时间戳（Presentation Time Stamp）
	*  @note 该方法会在Stream分发音频帧时被调用
	*  @note 音频帧通常不需要分片，一个RTP包即可
	*  @note 注释掉的代码（#if 0）是旧的发送方式，已被SendSrtpRtp替代
	*  @note AAC转OPUS功能暂时不支持，后期可能通过Muxer实现
	*/
	  void RtcConsumer::OnAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	  {
#if 0
		  // TODO@chensong  2025-10-24  AAC 转OPUS暂时不支持 后期支持
		  muxer_->EncodeAudio(frame);
#endif // 

		  auto  single_packet =
			  std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(&extension_manager_);
		  //GBMEDIASERVER_LOG(LS_INFO) << "audio size:" << frame.size() << ", pts: " << pts;
		  single_packet->SetPayloadType(sdp_.GetAudioPayloadType());
		  // 修复音频时间戳：毫秒转90kHz
		  // RTP timestamp = pts(ms) * 90kHz / 1000 = pts * 90 / 1000
		  single_packet->SetTimestamp(static_cast<uint32_t>(pts * 90));
		  single_packet->SetSsrc(sdp_.AudioSsrc());
		  single_packet->ReserveExtension<libmedia_transfer_protocol::TransportSequenceNumber>();
		  single_packet->SetMarker(true);
	 
		  single_packet->SetSequenceNumber(audio_seq_++);
		  single_packet->set_packet_type(libmedia_transfer_protocol::RtpPacketMediaType::kAudio);
		  single_packet->SetExtension<libmedia_transfer_protocol::TransportSequenceNumber>(
			  NextOutboundTransportSequenceNumber());

		  uint8_t* audio_ptr =  single_packet->AllocatePayload(frame.size());
		  if (audio_ptr)
		  {
			  memcpy(audio_ptr, frame.data(), frame.size());
			  single_packet->SetPayloadSize(frame.size());
		  }
#if 0
		  const uint8_t* data = single_packet->data();
		  size_t   len = single_packet->size();
		  if (!srtp_send_session_->EncryptRtp(&data, &len))
		  {
			 // continue;
			  return;
		  }


		  GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), rtc_remote_address_, rtc::PacketOptions());
		  //packets.push_back( std::move(single_packet));
#else 

		  //if (!outbound_loss_sim_.ShouldDropOutboundRtp())
		  {
			  SendSrtpRtp((uint8_t*)single_packet->data(), single_packet->size());
			  if (statistics_) {
				  statistics_->OnRtpPacketSent(false, single_packet->size());
			  }
		  }
#endif //
	  }

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理数据通道消息（On Data Channel）
	*  
	*  该方法用于发送数据通道消息到客户端。
	*  
	*  处理流程：
	*  1. 检查SCTP关联是否存在
	*  2. 调用SCTP关联的SendSctpMessage方法发送消息
	*  
	*  数据通道说明：
	*  - WebRTC数据通道基于SCTP协议
	*  - 数据通道可以传输任意二进制数据或文本数据
	*  - 数据通道支持可靠传输和不可靠传输
	*  
	*  @param params SCTP流参数，包含流ID、可靠性等信息
	*  @param ppid Payload Protocol Identifier，标识数据类型
	*  @param msg 消息数据缓冲区
	*  @param len 消息长度
	*  @note 该方法由Session在需要发送数据通道消息时调用
	*/
	  void RtcConsumer::OnDataChannel(
		  const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, 
		  uint32_t ppid, const uint8_t* msg, size_t len)
	  {
		  if (sctp_)
		  {
			  sctp_->SendSctpMessage(params, ppid, msg, len);
		  }
	  }

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联消息接收回调（On SCTP Association Message Received）
	*  
	*  该方法是SCTP关联接收到消息时的回调，用于处理数据通道消息。
	*  
	*  处理流程：
	*  1. 记录接收到的消息日志（流ID、PPID、长度、内容）
	*  2. 创建SCTP流参数，设置流ID
	*  3. 调用Session的AddDataChannel方法，将消息分发给所有消费者
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @param streamId SCTP流ID，标识数据通道
	*  @param ppid Payload Protocol Identifier，标识数据类型
	*  @param msg 消息数据缓冲区
	*  @param len 消息长度
	*  @note 该方法由SCTP关联在接收到消息时调用
	*  @note 消息会被分发给Session中的所有消费者
	*/
	  void RtcConsumer::OnSctpAssociationMessageReceived(
		  libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
		  uint16_t streamId,
		  uint32_t ppid,
		  const uint8_t* msg,
		  size_t len)
	  {
		  GBMEDIASERVER_LOG_T_F(LS_INFO) << " " << streamId << " " << ppid << " " << len << " "
			  << std::string((char*)msg, len);
		  libmedia_transfer_protocol::librtc::SctpStreamParameters params;
		  params.streamId = streamId;
		  GetSession()->AddDataChannel(params, ppid, msg, len);
	  }

	  void RtcConsumer::SendSenderReport()
	  {
		  if (!dtls_done_) {
			  return;
		  }

		  // 创建SR包
		  auto sr = std::make_unique<libmedia_transfer_protocol::rtcp::SenderReport>();
		  sr->SetSenderSsrc(sdp_.VideoSsrc());
		  
		  // 设置NTP时间戳
		  webrtc::NtpTime ntp_time = webrtc::Clock::GetRealTimeClock()->CurrentNtpTime();
		  sr->SetNtp(ntp_time);
		  
		  // 设置RTP时间戳、发送统计
		  sr->SetRtpTimestamp(sr_rtp_timestamp_);
		  sr->SetPacketCount(sr_packet_count_);
		  sr->SetOctetCount(sr_octet_count_);
		  
		  // 记录发送时间
		  last_sr_send_time_ms_ = rtc::SystemTimeMillis();

		  rtc::Buffer buffer = sr->Build();
		  
		  // 通过SRTP发送
		  SendSrtpRtcp(buffer.data(), buffer.size());
		  
		  GBMEDIASERVER_LOG(LS_INFO) << "SR sent: ssrc=" << sdp_.VideoSsrc() 
			  << ", packets=" << sr_packet_count_ 
			  << ", octets=" << sr_octet_count_;
	  }

	  void RtcConsumer::StartBandwidthProbe(int target_bitrate_bps)
	  {
		  if (!dtls_done_) {
			  return;
		  }
		  
		  // 探测码率 = 目标码率 * 1.5
		  int probe_bitrate_bps = static_cast<int>(target_bitrate_bps * 1.5);
		  
		  GBMEDIASERVER_LOG(LS_INFO) << "Starting bandwidth probe: target=" 
			  << target_bitrate_bps << " bps, probe=" << probe_bitrate_bps << " bps";
		  
		  // 注意：完整的带宽探测实现需要集成PacingController
		  // 当前为占位函数，待后续集成PacingController后完善
	  }

	  void RtcConsumer::OnTimer()
	  {
		 /* GbMediaService::GetInstance().worker_thread()->PostDelayedTask(ToQueuedTask(task_safety_,
			  [this]() {*/
				  if (!dtls_done_) {
					  return;
				  }

				  // 发送Sender Report
				  SendSenderReport();

				  // 获取当前目标码率
				  int32_t target_bps = bandwidth_estimation_.GetTargetBitrate();
				  
				  if (target_bps != current_target_bitrate_bps_ && target_bps > 0) {
					  current_target_bitrate_bps_ = target_bps;
					  OnBandwidthEstimationUpdate(target_bps);
				  }

				  // 定期更新统计数据
				  if (statistics_) {
					  statistics_->Update();
				  }

				  // 递归调用实现定时循环（每5秒）
				//  OnTimer();
			  //}), 5000);
	  }

	  void RtcConsumer::InitBandwidthEstimation()
	  {
		  bandwidth_estimation_.SetBitrates(100000, 10000000, 500000);
		  current_target_bitrate_bps_ = 500000;
		  GBMEDIASERVER_LOG(LS_INFO) << "Bandwidth estimation initialized with 500kbps";
	  }

	  void RtcConsumer::OnBandwidthEstimationUpdate(int32_t bitrate_bps)
	  {
		  GBMEDIASERVER_LOG(LS_INFO) << "Bandwidth estimate updated: " << bitrate_bps / 1000 << " kbps";
		  
		  // 更新统计信息
		  if (statistics_) {
			 // statistics_->SetTargetBitrate(bitrate_bps);
		  }
		  
		  // 这里可以通知编码器调整码率
		  // GetSession()->SetTargetBitrate(bitrate_bps);
	  }

}