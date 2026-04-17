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
				   date:  2025-10-18

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
 
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
#include "gb_media_server_log.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_util.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_packet_received.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/common_header.h"

#include "server/stream.h"
#include "producer/rtc_producer.h"
#include "server/session.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "libmedia_transfer_protocol/rtp_rtcp/video_rtp_depacketizer_h264.h"
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
#include "libmedia_transfer_protocol/rtp_rtcp/rtcp_packet/stream.h"
#include "share/statistics_manager.h"

namespace gb_media_server {


	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC生产者构造函数（RTC Producer Constructor）
	*  
	*  该构造函数用于创建RTC生产者实例，初始化WebRTC相关的组件，包括SDP、DTLS、接收缓冲区、NALU解析器等。
	*  RTC生产者负责接收客户端推送的音视频流，解密RTP包，解析H264 NALU单元，并将媒体帧分发给Stream。
	*  
	*  初始化流程：
	*  1. 调用基类RtcInterface和Producer的构造函数
	*  2. 创建任务安全标志（task_safety_），用于异步任务的生命周期管理
	*  3. 分配8MB的接收缓冲区（recv_buffer_），用于存储接收到的媒体数据
	*  4. 创建NALU解析器（nal_parse_），用于解析H264 NALU单元
	*  5. 创建RTCP接收上下文（rtcp_context_recv_），用于处理RTCP统计信息
	*  6. 连接DTLS信号槽，用于处理DTLS握手事件
	*  7. 设置SDP的本地DTLS指纹，用于DTLS握手验证
	*  8. 设置SDP的服务器地址和端口，从配置文件中读取
	*  9. 设置流名称，用于标识媒体流
	*  10. 配置RTP头部扩展，启用绝对发送时间和传输序列号
	*  
	*  DTLS信号槽连接：
	*  - SignalDtlsConnecting：DTLS连接中回调
	*  - SignalDtlsConnected：DTLS连接成功回调
	*  - SignalDtlsClose：DTLS连接关闭回调
	*  - SignalDtlsFailed：DTLS连接失败回调
	*  - SignalDtlsSendPakcet：DTLS发送数据包回调
	*  - SignalDtlsApplicationDataReceived：DTLS应用数据接收回调
	*  
	*  RTP头部扩展：
	*  - hasAbsoluteSendTime：绝对发送时间扩展，用于精确的时间同步
	*  - hasTransportSequenceNumber：传输序列号扩展，用于带宽估计和拥塞控制
	*  
	*  @param stream 流对象的共享指针，用于分发媒体帧
	*  @param s 会话对象的共享指针，用于管理会话状态
	*  @note 接收缓冲区大小为8MB，足以容纳大多数视频帧
	*  @note NALU解析器用于将RTP包中的H264 NALU单元组装为完整的视频帧
	*  @note RTCP上下文用于统计RTP包的接收情况，生成RTCP接收者报告
	*/
 
	RtcProducer::RtcProducer(
		const std::shared_ptr<Stream> & stream, 
		const std::shared_ptr<Session> &s)
		: RtcInterface(s)
		, Producer(  stream, s), 
		task_safety_(webrtc::PendingTaskSafetyFlag::CreateDetachedInactive()),

		recv_buffer_(new uint8_t[1024 * 1024 * 8])
	, recv_buffer_size_(0) 
		 ,nal_parse_(nullptr)
		, rtcp_context_recv_(new libmedia_transfer_protocol::librtcp::RtcpContextRecv())
		///,rtp_header_()
		//, extension_manager_()
	{
		//local_ufrag_ = GetUFrag(8);
		//local_passwd_ = GetUFrag(32);
		///uint32_t audio_ssrc = GetSsrc(10);
		//uint32_t video_ssrc = audio_ssrc + 1;

		//sdp_.SetLocalUFrag(local_ufrag_);
		//sdp_.SetLocalPasswd(local_passwd_);
		//sdp_.SetAudioSsrc(audio_ssrc);
		//sdp_.SetVideoSsrc(video_ssrc);
		//dtls_certs_.Init();
		//dtls_.Init();
		//dtls_.SignalDtlsSendPakcet.connect(this, &PlayRtcUser::OnDtlsSendPakcet);
		//dtls_.SignalDtlsHandshakeDone.connect(this, &PlayRtcUser::OnDtlsHandshakeDone);
		//dtls_.SignalDtlsClose.connect(this, &PlayRtcUser::OnDtlsClosed);


		dtls_.SignalDtlsConnecting.connect(this, &RtcProducer::OnDtlsConnecting);
		dtls_.SignalDtlsConnected.connect(this, &RtcProducer::OnDtlsConnected);
		dtls_.SignalDtlsClose.connect(this, &RtcProducer::OnDtlsClosed);
		dtls_.SignalDtlsFailed.connect(this, &RtcProducer::OnDtlsFailed);
		dtls_.SignalDtlsSendPakcet.connect(this, &RtcProducer::OnDtlsSendPakcet);
		dtls_.SignalDtlsApplicationDataReceived.connect(this, &RtcProducer::OnDtlsApplicationDataReceived);







		//sdp_.SetFingerprint(dtls_.Fingerprint());
		//sdp_.SetLocalFingerprint(libmedia_transfer_protocol::libssl::DtlsCerts::GetInstance().Fingerprints());
		//// 本地ip port 
		////sdp_.SetServerAddr(GbMediaService::GetInstance().RtpWanIp());
		////sdp_.SetServerPort(GbMediaService::GetInstance().RtpPort());
		//sdp_.SetServerAddr(YamlConfig::GetInstance().GetRtcServerConfig().ips.at(0));
		//sdp_.SetServerPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		//sdp_.SetServerExternAddr(YamlConfig::GetInstance().GetRtcServerConfig().extern_ip);
		//sdp_.SetServerExternPort(YamlConfig::GetInstance().GetRtcServerConfig().udp_port);
		//sdp_.SetStreamName(s->SessionName()/*s->SessionName()*/);
		nal_parse_ = libmedia_codec::NalParseFactory::Create( 
			libmedia_codec::ENalParseType::ENalH264Prase );;
		
		rtp_header_.extension.hasAbsoluteSendTime = true;
		rtp_header_.extension.hasTransportSequenceNumber = true;


		// 创建统计对象并注册到统计管理器（Create statistics object and register to manager）
		// @date 2025-10-18
		statistics_ = std::make_shared<ProducerStatistics>(
			s->SessionName(),
			s->SessionName(),
			"rtc"
		);
		StatisticsManager::GetInstance().RegisterProducer(s->SessionName(), statistics_);
		statistics_->SetState("created");
	
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC生产者析构函数（RTC Producer Destructor）
	*  
	*  该析构函数用于清理RTC生产者实例，释放所有相关资源。
	*  
	*  清理流程：
	*  1. 记录析构日志
	*  2. 删除SRTP发送会话，释放加密资源
	*  3. 删除SRTP接收会话，释放解密资源
	*  4. 设置dtls_done_标志为false，表示DTLS连接已关闭
	*  5. 从SRTP会话中移除视频和音频流（注意：此时srtp_send_session_已被删除，这里可能有bug）
	*  6. 断开所有DTLS信号槽连接
	*  7. 释放接收缓冲区（recv_buffer_）
	*  
	*  @note 必须先删除SRTP会话，再断开DTLS信号槽
	*  @note 流对象和会话对象由智能指针管理，会自动释放
	*  @note 代码中有潜在bug：在删除srtp_send_session_后又尝试调用其方法
	*/
	RtcProducer::~RtcProducer()
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);

		// 从统计管理器注销（Unregister from statistics manager）
		// @date 2025-10-18
		if (statistics_) {
			StatisticsManager::GetInstance().UnregisterProducer(GetSession()->SessionName());
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
		 
		if (srtp_send_session_)
		{
			srtp_send_session_->RemoveStream(sdp_.VideoSsrc());
			srtp_send_session_->RemoveStream(sdp_.AudioSsrc());
		}
		dtls_.disconnect_all();

		if (recv_buffer_)
		{
			recv_buffer_.reset();
			recv_buffer_ = nullptr;
		}
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 接收数据回调（On Receive）
	*  
	*  该方法用于处理从TCP连接接收到的数据。当前实现被注释掉（#if 0），
	*  因为WebRTC通常使用UDP传输，TCP传输较少使用。
	*  
	*  TCP传输处理流程（注释掉的代码）：
	*  1. 将接收到的数据追加到接收缓冲区
	*  2. 解析TCP包头，提取payload大小（2字节大端序）
	*  3. 检查缓冲区是否包含完整的包
	*  4. 判断包类型（RTP或RTCP）
	*  5. 解析RTP包或RTCP包
	*  6. 处理完整的包后，移动缓冲区指针
	*  7. 如果缓冲区还有剩余数据，继续解析
	*  
	*  TCP包格式：
	*  - 2字节：Payload大小（大端序）
	*  - N字节：Payload数据（RTP或RTCP包）
	*  
	*  @param buffer1 接收到的数据缓冲区
	*  @note 当前实现被注释掉，WebRTC通常使用UDP传输
	*  @note TCP传输需要处理粘包和拆包问题
	*/
	void RtcProducer::OnRecv(const rtc::CopyOnWriteBuffer&  buffer1)
	{
#if 0
		memcpy(recv_buffer_ + recv_buffer_size_, buffer1.data(), buffer1.size());
		recv_buffer_size_ += buffer1.size();
		//recv_buffer_.SetData(buffer1);
		int32_t   parse_size = 0;

		 
		while (recv_buffer_size_ - parse_size > 2)
		{ 
			int16_t  payload_size = libmedia_transfer_protocol::ByteReader<int16_t>::ReadBigEndian((&recv_buffer_[parse_size]));
			 
			if ((recv_buffer_size_ - parse_size) < (payload_size + 2))
			{
				// 当不不够一个完整包需要继续等待下一个包的到来
				//GBMEDIASERVER_LOG(LS_INFO) << "tcp tail small !!!  (read_bytes -parse_size:" << (recv_buffer_size_ - parse_size) << ") payload_size:" << payload_size;
				break;
			}
			parse_size += 2;  
			if (libmedia_transfer_protocol::IsRtpPacket(rtc::ArrayView<uint8_t>(recv_buffer_ + parse_size, payload_size)))
			{
				libmedia_transfer_protocol::RtpPacketReceived  rtp_packet_received; 
				bool ret = rtp_packet_received.Parse(recv_buffer_  + parse_size, payload_size);
				if (!ret)
				{ 
					GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed !!! size:" << (recv_buffer_size_ - parse_size); //<< "  , hex :" << rtc::hex_encode((const char *)(buffer.begin() + paser_size), (size_t)(read_bytes - paser_size));
				}
				else
				{
					//RTC_LOG(LS_INFO) << "rtp info :" << rtp_packet_received.ToString();
					if (rtp_packet_received.PayloadType() == 96)
					{
						//mpeg_decoder_->parse( rtp_packet_received.payload().data(), rtp_packet_received.payload_size());; 
					} 
				}
			}
			else if (libmedia_transfer_protocol::IsRtcpPacket(rtc::ArrayView<uint8_t>(recv_buffer_ + parse_size, payload_size/*read_bytes - paser_size*/)))
			{
				libmedia_transfer_protocol::rtcp::CommonHeader rtcp_block;  //rtcp_packet;
				bool ret = rtcp_block.Parse(recv_buffer_ + parse_size, payload_size/* read_bytes - paser_size*/);
				if (!ret)
				{
					GBMEDIASERVER_LOG(LS_WARNING) << "rtcp parse failed !!!";
				}
				//else
				{
					//parse_size += payload_size;
					//	RTC_LOG(LS_INFO) << "rtcp info :" << rtcp_block.ToString();
				}
			}
			else
			{
				GBMEDIASERVER_LOG(LS_ERROR) << " not know type --> : payload_size: " << payload_size;
				//parse_size += payload_size;
			}
			parse_size += payload_size;
 
		}
		//GBMEDIASERVER_LOG(LS_INFO) << "read_bytes:" << recv_buffer_size_ << ", parse_size:" << parse_size;
		if (recv_buffer_size_ - parse_size > 0)
		{
			//memcpy((char *)recv_buffer_.begin(), buffer.data() + parse_size, (buffer.size() - parse_size));
			recv_buffer_size_ -= parse_size;;
			memmove(recv_buffer_, recv_buffer_+ parse_size,recv_buffer_size_);
			 
		}
		else
		{
			recv_buffer_size_ = 0;
			parse_size = 0;
			//memcpy((char *)recv_buffer_.begin(), buffer.begin() + parse_size, (read_bytes - parse_size));
			//recv_buffer_size_ = read_bytes - parse_size;
		}
		
#endif // recv_buffer_size_
	}





	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 定时器回调（On Timer）
	*  
	*  该方法用于定期发送RTCP接收者报告（RR），用于反馈接收质量。
	*  
	*  处理流程：
	*  1. 使用任务安全标志（task_safety_）投递延迟任务到工作线程
	*  2. 检查DTLS握手是否完成，未完成则直接返回
	*  3. 使用RTCP上下文创建RTCP RR包
	*  4. 调用SendSrtpRtcp方法发送RTCP RR包
	*  5. 更新RTCP RR时间戳
	*  6. 切换流状态（用于测试）
	*  7. 递归调用OnTimer，实现定时循环（每5秒执行一次）
	*  
	*  RTCP RR说明：
	*  - RR（Receiver Report）用于反馈接收质量
	*  - RR包含丢包率、抖动、往返时延等统计信息
	*  - RR通常每隔几秒发送一次
	*  
	*  任务安全标志：
	*  - task_safety_用于管理异步任务的生命周期
	*  - 当对象销毁时，未执行的任务会被自动取消
	*  - 避免在对象销毁后执行回调导致崩溃
	*  
	*  @note 该方法会递归调用自己，实现定时循环
	*  @note 定时间隔为5秒
	*  @note 流状态切换用于测试，实际使用时可能需要移除
	*/
	void RtcProducer::OnTimer()
	{
		/*
		 network_thread_->PostDelayedTask(
        ToQueuedTask(task_safety_,
                     [this, recheck = *result.recheck_event]() {
                       SortConnectionsAndUpdateState(recheck);
                     }),
        result.recheck_event->recheck_delay_ms);
		*/
#if 1
		gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayedTask(ToQueuedTask(task_safety_,
		// gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayed(RTC_FROM_HERE, 
			[this]() {
				if (!dtls_done_)
				{

					return ;
				}
				//if (rtc::SystemTimeMillis() - rtcp_rr_timestamp_ > 4000)
				{
					rtc::Buffer buffer = rtcp_context_recv_->createRtcpRR(sdp_.VideoSsrc(), sdp_.VideoSsrc());
					GBMEDIASERVER_LOG(LS_INFO) << "Send RR";
					SendSrtpRtcp(buffer.data(), buffer.size());
					rtcp_rr_timestamp_ = rtc::SystemTimeMillis();

					// 统计RTCP RR发送（Statistics RTCP RR sent）
					// @date 2025-10-18
					if (statistics_) {
						statistics_->OnRtcpRRSent();
					}
				}
				{
				
					stream_status_ = stream_status_? false:true;
				//	SetStreamStatus(stream_status_);
				}

				// 定期更新统计数据（Update statistics periodically）
				// @date 2025-10-18
				if (statistics_) {
					statistics_->Update();
				}
 
			OnTimer();
		}), 5000);

#endif //
		// PostDelayed(RTC_FROM_HERE, milliseconds, &queued_task_handler_,
		/*id=*///0,
		//	new ScopedMessageData<webrtc::QueuedTask>(std::move(task)));
	//	gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayed(RTC_FROM_HERE, 5000,   this, );

	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理Offer SDP（Process Offer SDP）
	*  
	*  该方法用于处理客户端发送的SDP Offer，解析媒体能力、ICE候选、DTLS指纹等信息。
	*  
	*  @param rtc_sdp_type SDP类型（Offer或Answer）
	*  @param sdp SDP字符串，包含完整的SDP描述
	*  @return 解析成功返回true，失败返回false
	*/
	bool RtcProducer::ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp) {
		sdp_.SetSdpType(rtc_sdp_type);
		bool result = sdp_.Decode(sdp);

		// 设置媒体信息到统计对象（Set media info to statistics）
		// @date 2025-10-18
		if (result && statistics_) {
			// 设置视频信息
			statistics_->SetVideoInfo(
				sdp_.VideoSsrc(),
				0, // width未知
				0, // height未知
				"H264" // 假设为H264编解码器
			);

			// 设置音频信息
			statistics_->SetAudioInfo(
				sdp_.AudioSsrc(),
				48000, // 假设采样率为48kHz
				2, // 假设为立体声
				"OPUS" // 假设为OPUS编解码器
			);
		}

		return result;
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 获取本地ICE用户名片段（Local UFrag）
	*  
	*  @return 本地ICE用户名片段
	*/
	const std::string& RtcProducer::LocalUFrag() const {
		return sdp_.GetLocalUFrag();
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 获取本地ICE密码（Local Password）
	*  
	*  @return 本地ICE密码
	*/
	const std::string& RtcProducer::LocalPasswd() const {
		return sdp_.GetLocalPasswd();
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 获取远程ICE用户名片段（Remote UFrag）
	*  
	*  @return 远程ICE用户名片段
	*/
	const std::string& RtcProducer::RemoteUFrag() const {
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
	*/
	std::string RtcProducer::BuildAnswerSdp() {
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
	*  @note 该方法通常在ICE连接建立后调用
	*  @note DTLS握手成功后会触发OnDtlsConnected回调
	*/
	void RtcProducer::MayRunDtls()
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
		GBMEDIASERVER_LOG(LS_INFO) << "remote role:" << role << " , local_role :" << (int32_t)local_role;
		dtls_.Run(local_role);
	}

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
	void RtcProducer::OnDtlsRecv(const uint8_t* buf, size_t size)
	{
		dtls_.OnRecv(buf, size);
	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理SRTP RTP包（On SRTP RTP）
	*  
	*  该方法用于处理从客户端接收到的SRTP RTP包，解密后解析RTP头部，提取媒体数据，
	*  并将其分发给Stream。这是RTC生产者的核心方法，负责接收客户端推送的音视频流。
	*  
	*  处理流程：
	*  1. 使用SRTP接收会话解密RTP包
	*  2. 解析RTP包，提取RTP头部信息（序列号、时间戳、SSRC、Payload Type等）
	*  3. 提取RTP头部扩展（传输序列号、绝对发送时间等）
	*  4. 如果包含传输序列号扩展，更新TWCC上下文（用于带宽估计）
	*  5. 根据Payload Type判断是音频还是视频：
	*     - 音频：直接将RTP Payload传递给Stream的AddAudioFrame方法
	*     - 视频：使用NALU解析器解析H264 NALU单元
	*  6. 如果RTP包的Marker位为true，表示视频帧结束：
	*     - 创建EncodedImage对象，包含完整的视频帧
	*     - 将视频帧传递给Stream的AddVideoFrame方法
	*     - 清空NALU解析器的缓冲区，准备接收下一帧
	*  7. 更新RTCP接收上下文，用于生成RTCP接收者报告
	*  
	*  RTP包解析：
	*  - RTP头部包含序列号、时间戳、SSRC、Payload Type等信息
	*  - RTP扩展包含传输序列号、绝对发送时间等信息
	*  - RTP Payload包含音频或视频编码数据
	*  
	*  H264 NALU解析：
	*  - H264视频帧由多个NALU单元组成
	*  - 每个RTP包可能包含一个完整的NALU、NALU的一部分（FU-A分片）或多个NALU（STAP-A聚合）
	*  - NALU解析器负责将RTP包中的NALU单元组装为完整的视频帧
	*  - Marker位为true时，表示视频帧的最后一个RTP包
	*  
	*  TWCC（Transport Wide Congestion Control）：
	*  - TWCC用于带宽估计和拥塞控制
	*  - 传输序列号扩展用于标识RTP包的发送顺序
	*  - TWCC上下文记录RTP包的接收时间，用于计算往返时延和丢包率
	*  
	*  时间戳转换：
	*  - 音频时间戳：RTP时间戳 / 90（转换为毫秒）
	*  - 视频时间戳：RTP时间戳 / 90（转换为毫秒）
	*  
	*  @param data RTP数据包缓冲区（已加密）
	*  @param size 数据包大小
	*  @note 该方法由RtcService在接收到RTP包时调用
	*  @note RTP包必须先通过SRTP解密才能处理
	*  @note 视频帧会被缓存在NALU解析器中，直到Marker位为true才会分发
	*  @note 注释掉的代码（#if 0）用于将视频帧保存到文件，用于调试
	*/
	void RtcProducer::OnSrtpRtp(  uint8_t* data, size_t size)
	{
		//GBMEDIASERVER_LOG_T_F(LS_INFO);
		if (!srtp_recv_session_->DecryptSrtp((uint8_t*)data, (size_t*)&size))
		{
			GBMEDIASERVER_LOG_T_F(LS_WARNING) << "decrypt srtp failed  size : " <<size<<" !!!";
			return;
		}


		libmedia_transfer_protocol::RtpPacketReceived  rtp_packet_received(&extension_manager_);
		bool ret = rtp_packet_received.Parse(data , size);
		if (!ret)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "rtp parse failed !!! size:" << size; //<< "  , hex :" << rtc::hex_encode((const char *)(buffer.begin() + paser_size), (size_t)(read_bytes - paser_size));
		}
		else
		{
			
			rtp_packet_received.GetHeader(&rtp_header_);
			//GBMEDIASERVER_LOG(LS_INFO) << rtp_header_.ToString() ;
		

			if (rtp_header_.extension.hasTransportSequenceNumber)
			{
				twcc_context_.onRtp(rtp_header_.ssrc, 
					rtp_header_.extension.transportSequenceNumber, 
					rtc::SystemTimeMillis());
			}

			// 判断是视频还是音频包（Determine if video or audio packet）
			// @date 2025-10-18
			bool is_video = (rtp_packet_received.PayloadType() == sdp_.GetVideoPayloadType());

			// 统计RTP包接收（Statistics RTP packet received）
			// @date 2025-10-18
			if (statistics_) {
				statistics_->OnRtpPacketReceived(is_video, size);
			}

			if (rtp_packet_received.PayloadType() != sdp_.GetVideoPayloadType()) 
			{
				
				if (rtp_packet_received.PayloadType() == sdp_.GetAudioPayloadType())
				{

					GetStream()->AddAudioFrame(rtc::CopyOnWriteBuffer(rtp_packet_received.payload().data(), rtp_packet_received.payload_size())
						, rtp_packet_received.Timestamp()/90);
				}
				else
				{
					 GBMEDIASERVER_LOG(LS_INFO) << "payload_type:" << rtp_packet_received.PayloadType()
				 	<< ", ssrc:" << rtp_packet_received.Ssrc() << ", video payload type:" << sdp_.GetVideoPayloadType() 
						 ;

				}
				return;
			}
			/*RTC_LOG(LS_INFO) << "rtp info :" << rtp_packet_received.PayloadType() 
				<< ", seq:" << rtp_packet_received.SequenceNumber()
				<< ", masker:" << rtp_packet_received.Marker();*/
			if (rtcp_context_recv_ && rtp_packet_received.Ssrc() == sdp_.VideoSsrc())
			{
				// ntp_stamp : getStamp() * uint64_t(1000) / sample_rate
				rtcp_context_recv_->onRtp(rtp_packet_received.SequenceNumber(), rtp_packet_received.Timestamp(),
					rtp_packet_received.Timestamp()/ 90000, 90000,rtp_packet_received.payload_size());
			}
			//GBMEDIASERVER_LOG(LS_INFO) << " ssrc:" << rtp_packet_received.Ssrc() << ", payload_type:" << rtp_packet_received.PayloadType() << ", seq:" << rtp_packet_received.SequenceNumber()
			//	<< ", marker:" << rtp_packet_received.Marker() << ", payload_size:" << rtp_packet_received.payload_size();
			//memcpy(recv_buffer_ + recv_buffer_size_, rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
			//recv_buffer_size_ += rtp_packet_received.payload_size();
			nal_parse_->parse_packet(rtp_packet_received.payload().data(), rtp_packet_received.payload_size());
			if (rtp_packet_received.Marker())
			{
				 
				libmedia_codec::EncodedImage encode_image;
				encode_image.SetTimestamp(rtp_packet_received.Timestamp()/90);
				encode_image.SetEncodedData(
					libmedia_codec::EncodedImageBuffer::Create(
						nal_parse_->buffer_stream_,
						nal_parse_->buffer_index_
						//video_parse->video_payload.data(),
						//video_parse->video_payload.size()
					));

#if 0

				static FILE* out_file_ptr = fopen("rtc_push.h264", "wb+");
				if (out_file_ptr)
				{
					char start_code[4] = {0x00, 0x00, 0x00, 0x01};
					//fwrite(start_code, 1, sizeof(start_code), out_file_ptr);
					fwrite(nal_parse_->buffer_stream_, 1, nal_parse_->buffer_index_, out_file_ptr);
					//fwrite(video_parse->video_payload.data(), 1, video_parse->video_payload.size(), out_file_ptr);
					fflush(out_file_ptr);
				}

#endif 
				//decoder_->Decode(encode_image, true, 1);

				// 统计视频帧接收（Statistics video frame received）
				// @date 2025-10-18
				if (statistics_) {
					// 简单判断是否为关键帧（通过检查NALU类型）
					bool is_key_frame = false;
					if (nal_parse_->buffer_index_ > 4) {
						uint8_t nal_type = nal_parse_->buffer_stream_[4] & 0x1F;
						is_key_frame = (nal_type == 5); // IDR帧
					}
					statistics_->OnFrameReceived(true, is_key_frame);
				}

				GetStream()->AddVideoFrame(std::move(encode_image));
				nal_parse_->buffer_index_ = 0;
				//decoder_->Decode();
			}
		}

	}
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 处理SRTP RTCP包（On SRTP RTCP）
	*  
	*  该方法用于处理从客户端接收到的SRTP RTCP包，解密后解析控制信息。
	*  
	*  处理流程：
	*  1. 使用SRTP接收会话解密RTCP包
	*  2. 解析RTCP复合包，可能包含多个RTCP块
	*  3. 遍历所有RTCP块，根据类型进行不同处理：
	*     - SR（Sender Report）：发送者报告，更新RTCP接收上下文
	*     - RR（Receiver Report）：接收者报告
	*     - SDES（Source Description）：源描述信息，包含CNAME等
	*     - BYE：会话结束通知
	*     - APP：应用定义的RTCP包
	*     - RTPFB（RTP Feedback）：RTP反馈（当前未处理）
	*     - PSFB（Payload-Specific Feedback）：负载特定反馈（当前未处理）
	*  4. 统计跳过的RTCP包数量
	*  
	*  RTCP包类型处理：
	*  - SR（200）：发送者报告，包含发送统计信息，更新RTCP上下文
	*  - RR（201）：接收者报告，包含接收统计信息
	*  - SDES（202）：源描述信息，包含CNAME、NAME等
	*  - BYE（203）：会话结束
	*  - APP（204）：应用定义
	*  - RTPFB（205）：RTP反馈（NACK、TMMBR等）
	*  - PSFB（206）：负载特定反馈（PLI、FIR等）
	*  
	*  RTCP上下文更新：
	*  - 接收到SR包时，更新RTCP接收上下文
	*  - RTCP上下文用于生成RTCP接收者报告（RR）
	*  - RTCP上下文记录发送者的NTP时间戳、RTP时间戳等信息
	*  
	*  @param data RTCP数据包缓冲区（已加密）
	*  @param size 数据包大小
	*  @note 该方法由RtcService在接收到RTCP包时调用
	*  @note RTCP包必须先通过SRTP解密才能处理
	*  @note 当前实现只处理SR和SDES包，其他类型的RTCP包被跳过
	*  @note 注释掉的代码用于处理其他类型的RTCP包，可能在未来启用
	*/
	void RtcProducer::OnSrtpRtcp(  uint8_t* data, size_t size)
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
					return ;
				}
				 ++num_skipped_packets_;
				break;
			}

			//if (packet_type_counter_.first_packet_time_ms == -1)
			//	packet_type_counter_.first_packet_time_ms = clock_->TimeInMilliseconds();
			//RTC_LOG_F(LS_INFO) << "recvice RTCP TYPE = " << rtcp_block.type();
			switch (rtcp_block.type()) {
			case libmedia_transfer_protocol::rtcp::SenderReport::kPacketType:
			{
				//RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type();
				//HandleSenderReport(rtcp_block, packet_information);
				// 
				//received_blocks[packet_information->remote_ssrc].sender_report = true;
				libmedia_transfer_protocol::rtcp::SenderReport sender_report;
				if (!sender_report.Parse(rtcp_block)) {
					++num_skipped_packets_;
					return;
				}

				RTC_LOG_F(LS_INFO) << "recvice SR RTCP TYPE = " << rtcp_block.type() 
					<< ", ssrc:" << sender_report.sender_ssrc();

				// 统计RTCP SR接收（Statistics RTCP SR received）
				// @date 2025-10-18
				if (statistics_) {
					statistics_->OnRtcpSRReceived();
				}

				if (rtcp_context_recv_ && sender_report.sender_ssrc() == sdp_.VideoSsrc())
				{
					rtcp_context_recv_->onRtcp(&sender_report);
				}
				break;
			}
			case libmedia_transfer_protocol::rtcp::ReceiverReport::kPacketType:
				   RTC_LOG_F(LS_INFO) << "recvice RR RTCP TYPE = " << rtcp_block.type();
				//HandleReceiverReport(rtcp_block, packet_information);
				break;
			case libmedia_transfer_protocol::rtcp::Sdes::kPacketType:
			{
				//RTC_LOG(LS_INFO) << "recvice SDES RTCP TYPE = " << rtcp_block.type();
				//HandleSdes(rtcp_block, packet_information);
				libmedia_transfer_protocol::rtcp::Sdes sdes;
				if (!sdes.Parse(rtcp_block)) {
					++num_skipped_packets_;

				}
				else
				{
					for (const libmedia_transfer_protocol::rtcp::Sdes::Chunk& chunk : sdes.chunks()) {
						RTC_LOG(LS_INFO) << "recvice SDES RTCP  ssrc: " << chunk.ssrc << ", cname:" << chunk.cname;
					}
				}
				break; 
			}
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
				RTC_LOG_F(LS_INFO) << "recvice rtpfb ";
				//switch (rtcp_block.fmt()) {
				//case rtcp::Nack::kFeedbackMessageType:
				//	//  RTC_LOG_F(LS_INFO) << "recvice rtpfb  nack RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleNack(rtcp_block, packet_information);
				//	break;
				//case rtcp::Tmmbr::kFeedbackMessageType:
				//	RTC_LOG(LS_INFO) << "recvice rtpfb  tmmbr RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleTmmbr(rtcp_block, packet_information);
				//	break;
				//case rtcp::Tmmbn::kFeedbackMessageType:
				//	RTC_LOG(LS_INFO) << "recvice rtpfb tmmbn RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleTmmbn(rtcp_block, packet_information);
				//	break;
				//case rtcp::RapidResyncRequest::kFeedbackMessageType:
				//	RTC_LOG(LS_INFO) << "recvice rtpfb rapidresy ync  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleSrReq(rtcp_block, packet_information);
				//	break;
				//case rtcp::TransportFeedback::kFeedbackMessageType:
				//	//  RTC_LOG_F(LS_INFO) << "recvice rtpfb transport feedback  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
				//	HandleTransportFeedback(rtcp_block, packet_information);
				//	break;
				//default:
				//	++num_skipped_packets_;
				//	break;
				//}
				break;
			case libmedia_transfer_protocol::rtcp::Psfb::kPacketType:
				RTC_LOG(LS_INFO) << "recvice psfb  pli";
				/*switch (rtcp_block.fmt()) {
				case rtcp::Pli::kFeedbackMessageType:
					RTC_LOG(LS_INFO) << "recvice psfb  pli  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
					HandlePli(rtcp_block, packet_information);
					break;
				case rtcp::Fir::kFeedbackMessageType:
					RTC_LOG(LS_INFO) << "recvice psfb  fir  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
					HandleFir(rtcp_block, packet_information);
					break;
				case rtcp::Psfb::kAfbMessageType:
					RTC_LOG(LS_INFO) << "recvice psfb  psfb  af  RTCP TYPE = " << rtcp_block.type() << ", sub_type = " << rtcp_block.fmt();
					HandlePsfbApp(rtcp_block, packet_information);
					break;
				default:
					++num_skipped_packets_;
					break;
				}*/
				break;
			default:
				 ++num_skipped_packets_;
				break;
			}
		}
		//OnRecv(data, size);
	}

#if 0
	std::string RtcProducer::GetUFrag(int size) {
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
	uint32_t RtcProducer::GetSsrc(int size)
	{
		static std::mt19937 mt{ std::random_device{}() };
		static std::uniform_int_distribution<> rand(10000000, 99999999);

		return rand(mt);
	}
#endif // 

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 请求关键帧（Request Key Frame）
	*  
	*  该方法用于向客户端请求发送关键帧（IDR帧）。当服务器需要快速恢复播放
	*  或新的消费者加入时，会调用该方法请求关键帧。
	*  
	*  处理流程：
	*  1. 检查距离上次请求关键帧的时间，如果小于3秒则直接返回（避免频繁请求）
	*  2. 更新请求关键帧的时间戳，设置为当前时间 + 3秒
	*  3. 创建RTCP PLI（Picture Loss Indication）包
	*  4. 设置PLI包的发送者SSRC和媒体SSRC
	*  5. 将PLI包添加到RTCP复合包中
	*  6. 构建RTCP复合包
	*  7. 调用SendSrtpRtcp方法发送RTCP PLI包
	*  
	*  关键帧请求方式：
	*  - RTCP PLI（Picture Loss Indication）：图像丢失指示，请求关键帧
	*  - RTCP FIR（Full Intra Request）：完整帧内请求，请求关键帧（注释掉的代码）
	*  - SIP Info消息：通过SIP信令请求关键帧（未实现）
	*  
	*  PLI vs FIR：
	*  - PLI：简单的关键帧请求，不包含序列号
	*  - FIR：包含序列号的关键帧请求，可以跟踪请求状态
	*  - PLI更常用，FIR较少使用
	*  
	*  请求频率限制：
	*  - 为了避免频繁请求关键帧，设置了3秒的冷却时间
	*  - 频繁请求关键帧会增加带宽消耗和编码器负担
	*  
	*  @note 该方法通常在新的消费者加入或网络恢复后调用
	*  @note 请求频率限制为每3秒一次
	*  @note 注释掉的代码用于发送FIR请求，可能在未来启用
	*/
	void RtcProducer::RequestKeyFrame()
	{
		static const int64_t    RequestKeyFrameMs = 500;

		int64_t  diff_ms = rtc::TimeMillis() - request_key_frame_;
		if (diff_ms < RequestKeyFrameMs)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "session: session_->SessionName() "<< "request key frame diff_ms : " << diff_ms;
			return;
		}
		request_key_frame_ = rtc::TimeMillis();
		///////////////////////////////////////////////////////////////////////////
	////                         IDR Request

	//     关键帧也叫做即时刷新帧，简称IDR帧。对视频来说，IDR帧的解码无需参考之前的帧，因此在丢包严重时可以通过发送关键帧请求进行画面的恢复。
	// 关键帧的请求方式分为三种：RTCP FIR反馈（Full intra frame request）、RTCP PLI 反馈（Picture Loss Indictor）或SIP Info消息，
	//							具体使用哪种可通过协商确定.

	///////////////////////////////////////////////////////////////////////////
		//if (this->params.usePli)
		{

			std::unique_ptr< libmedia_transfer_protocol::rtcp::Pli> pli = std::make_unique< libmedia_transfer_protocol::rtcp::Pli>();
			pli->SetSenderSsrc(sdp_.VideoSsrc());
			pli->SetMediaSsrc(sdp_.VideoSsrc());
			GBMEDIASERVER_LOG(LS_INFO) << " send rquest pli ";
			libmedia_transfer_protocol::rtcp::CompoundPacket compound;               // Builds a compound RTCP packet with
		    compound.Append(std::move(pli));                  // a receiver report, report block
		   // compound.Append(&fir);                 // and fir message.
		    rtc::Buffer packet = compound.Build();
			SendSrtpRtcp(packet.data(), packet.size());
			//SendImmediateFeedback(pli);
			//libmedia_transfer_protocol::rtcp::TransportFeedback  
			//MS_DEBUG_2TAGS(rtcp, rtx, "sending PLI [ssrc:%" PRIu32 "]", GetSsrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			//RTC::RTCP::FeedbackPsPliPacket packet(GetSsrc(), GetSsrc());

			//packet.Serialize(RTC::RTCP::Buffer);

			//this->pliCount++;

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
		//else if (this->params.useFir)
		{
			//MS_DEBUG_2TAGS(rtcp, rtx, "sending FIR [ssrc:%" PRIu32 "]", GetSsrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			//RTC::RTCP::FeedbackPsFirPacket packet(GetSsrc(), GetSsrc());
			//auto* item = new RTC::RTCP::FeedbackPsFirItem(GetSsrc(), ++this->firSeqNumber);
			//
			//packet.AddItem(item);
			//packet.Serialize(RTC::RTCP::Buffer);
			//
			//this->firCount++;

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 设置流状态（Set Stream Status）
	*  
	*  该方法用于向客户端发送流状态通知，告知客户端流是否可用。
	*  
	*  处理流程：
	*  1. 创建RTCP Stream状态包
	*  2. 设置发送者SSRC
	*  3. 设置流状态（true表示流可用，false表示流不可用）
	*  4. 将Stream状态包添加到RTCP复合包中
	*  5. 构建RTCP复合包
	*  6. 调用SendSrtpRtcp方法发送RTCP Stream状态包
	*  
	*  流状态说明：
	*  - true：流可用，客户端可以正常接收媒体数据
	*  - false：流不可用，客户端应停止接收媒体数据
	*  
	*  使用场景：
	*  - 流开始时，发送true通知客户端流已就绪
	*  - 流暂停时，发送false通知客户端流已暂停
	*  - 流恢复时，发送true通知客户端流已恢复
	*  
	*  @param status 流状态（true表示可用，false表示不可用）
	*  @note 该方法用于通知客户端流的可用性
	*  @note 客户端收到状态通知后，可以相应地调整播放策略
	*/
	void RtcProducer::SetStreamStatus(bool status)
	{
		std::unique_ptr< libmedia_transfer_protocol::rtcp::Stream> stream = std::make_unique< libmedia_transfer_protocol::rtcp::Stream>();
		stream->SetSenderSsrc(sdp_.VideoSsrc());
	//	stream->setm(sdp_.VideoSsrc());
		stream->SetStatus(status);
		GBMEDIASERVER_LOG(LS_INFO) << " set  ssrc:"<<sdp_.VideoSsrc() <<", send stream status  :" << status;
		libmedia_transfer_protocol::rtcp::CompoundPacket compound;               // Builds a compound RTCP packet with
		compound.Append(std::move(stream));                  // a receiver report, report block
	   // compound.Append(&fir);                 // and fir message.
		rtc::Buffer packet = compound.Build();
		SendSrtpRtcp(packet.data(), packet.size());
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 数据通道消息发送（On Data Channel）
	*  
	*  该方法用于发送数据通道消息到客户端。
	*  
	*  处理流程：
	*  1. 检查SCTP关联是否存在
	*  2. 调用SCTP关联的SendSctpMessage方法发送消息
	*  
	*  @param params SCTP流参数，包含流ID、可靠性等信息
	*  @param ppid Payload Protocol Identifier，标识数据类型
	*  @param msg 消息数据缓冲区
	*  @param len 消息长度
	*  @note 该方法由Session在需要发送数据通道消息时调用
	*/
	void RtcProducer::OnDataChannel(
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
	void RtcProducer::OnSctpAssociationMessageReceived(
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
	 
}