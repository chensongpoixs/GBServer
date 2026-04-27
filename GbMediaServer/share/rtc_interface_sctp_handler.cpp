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
#include "share/rtc_interface.h"

namespace gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联连接中回调（On SCTP Association Connecting）
	*  
	*  该方法是SCTP关联连接过程中的回调，当SCTP开始连接时触发。
	*  
	*  SCTP连接流程：
	*  1. SCTP关联开始连接（触发该回调）
	*  2. SCTP发送INIT包
	*  3. 接收INIT-ACK包
	*  4. 发送COOKIE-ECHO包
	*  5. 接收COOKIE-ACK包
	*  6. SCTP连接建立成功（触发OnSctpAssociationConnected回调）
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @note 该方法由SCTP关联在连接开始时调用
	*  @note 可以在此记录日志或更新连接状态
	*/
	void RtcInterface::OnSctpAssociationConnecting(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联连接成功回调（On SCTP Association Connected）
	*  
	*  该方法是SCTP关联连接成功后的回调，当SCTP连接建立完成时触发。
	*  
	*  连接成功后的操作：
	*  - 可以开始发送和接收数据通道消息
	*  - 可以创建新的数据通道
	*  - 可以通知上层应用连接已就绪
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @note 该方法由SCTP关联在连接成功时调用
	*  @note 连接成功后，可以开始使用数据通道
	*/
	void RtcInterface::OnSctpAssociationConnected(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联连接失败回调（On SCTP Association Failed）
	*  
	*  该方法是SCTP关联连接失败时的回调，当SCTP连接建立失败时触发。
	*  
	*  连接失败原因：
	*  - 网络不可达
	*  - 连接超时
	*  - 对端拒绝连接
	*  - SCTP参数不匹配
	*  
	*  失败后的处理：
	*  - 记录错误日志
	*  - 通知上层应用连接失败
	*  - 清理相关资源
	*  - 可能需要重新建立连接
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @note 该方法由SCTP关联在连接失败时调用
	*  @note 连接失败后，需要重新建立SCTP关联
	*/
	void RtcInterface::OnSctpAssociationFailed(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)   
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联连接关闭回调（On SCTP Association Closed）
	*  
	*  该方法是SCTP关联连接关闭时的回调，当SCTP连接正常关闭时触发。
	*  
	*  关闭流程：
	*  1. 一方发送SHUTDOWN包
	*  2. 另一方发送SHUTDOWN-ACK包
	*  3. 发起方发送SHUTDOWN-COMPLETE包
	*  4. SCTP连接关闭（触发该回调）
	*  
	*  关闭后的处理：
	*  - 记录关闭日志
	*  - 通知上层应用连接已关闭
	*  - 清理相关资源
	*  - 停止发送和接收数据通道消息
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @note 该方法由SCTP关联在连接关闭时调用
	*  @note 正常关闭不会触发错误，只是清理资源
	*/
	void RtcInterface::OnSctpAssociationClosed(
		libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation)   
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联发送数据回调（On SCTP Association Send Data）
	*  
	*  该方法是SCTP关联请求发送数据时的回调，用于将SCTP数据包通过DTLS发送。
	*  
	*  处理流程：
	*  1. SCTP关联生成SCTP数据包
	*  2. 调用该回调，请求发送数据包
	*  3. 将SCTP数据包通过DTLS的SendApplicationData方法发送
	*  4. DTLS加密SCTP数据包
	*  5. 通过UDP socket发送加密后的数据包
	*  
	*  SCTP over DTLS：
	*  - WebRTC使用SCTP over DTLS传输数据通道消息
	*  - DTLS提供加密和认证
	*  - SCTP提供可靠或不可靠的消息传输
	*  - SCTP数据包作为DTLS应用数据发送
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @param data SCTP数据包缓冲区
	*  @param len 数据包长度
	*  @note 该方法由SCTP关联在需要发送数据时调用
	*  @note SCTP数据包会通过DTLS加密后发送
	*/
	void RtcInterface::OnSctpAssociationSendData(
	libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
	const uint8_t* data, size_t len)
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		// 检查是否正在销毁，防止析构后 SCTP 回调访问已释放的 DTLS 对象导致崩溃
		if (destoy_ || !dtls_done_)
		{
			return;
		}
		dtls_.SendApplicationData(data, len);
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief SCTP关联消息接收回调（On SCTP Association Message Received）
	*  
	*  该方法是SCTP关联接收到消息时的回调，用于处理数据通道消息。
	*  
	*  处理流程：
	*  1. SCTP关联接收到SCTP数据包
	*  2. 解析SCTP流ID和PPID
	*  3. 调用该回调，传递消息内容
	*  4. 记录接收到的消息日志（流ID、PPID、长度、内容）
	*  5. 创建SCTP流参数，设置流ID
	*  6. 注释掉的代码用于回显消息（测试用）
	*  
	*  数据通道消息说明：
	*  - WebRTC数据通道可以传输任意二进制数据或文本数据
	*  - 每个数据通道对应一个SCTP流
	*  - 流ID用于标识数据通道
	*  - PPID用于标识数据类型（字符串、二进制、空消息等）
	*  
	*  PPID类型：
	*  - 51：WebRTC String（UTF-8文本）
	*  - 52：WebRTC Binary Partial（二进制数据，部分）
	*  - 53：WebRTC Binary（二进制数据，完整）
	*  - 54：WebRTC String Partial（UTF-8文本，部分）
	*  - 56：WebRTC String Empty（空字符串）
	*  - 57：WebRTC Binary Empty（空二进制数据）
	*  
	*  @param sctpAssociation SCTP关联对象指针
	*  @param streamId SCTP流ID，标识数据通道
	*  @param ppid Payload Protocol Identifier，标识数据类型
	*  @param msg 消息数据缓冲区
	*  @param len 消息长度
	*  @note 该方法由SCTP关联在接收到消息时调用
	*  @note 注释掉的代码用于回显消息，可用于测试数据通道
	*  @note 实际应用中，应将消息传递给上层应用处理
	*/
	void RtcInterface::OnSctpAssociationMessageReceived(
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

#if 0 // test echo hello 
		if (sctp_) {
			// 回显数据  [AUTO-TRANSLATED:7868d3a4]
			// Echo data
			sctp_->SendSctpMessage(params, ppid, msg, len);
		}
#endif //
	}


}


