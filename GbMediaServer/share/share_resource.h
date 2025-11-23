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
#ifndef _C_RTC_SHARE_RESOURCE_H_
#define _C_RTC_SHARE_RESOURCE_H_

#include <string>
#include <cstdint>
#include <memory>   
#include <vector>  
#include <string>

#include <atomic>

#include "rtc_base/copy_on_write_buffer.h"

#include "libmedia_codec/encoded_image.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
namespace  gb_media_server
{

	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 共享资源类型枚举（Share Resource Type Enum）
	*  
	*  该枚举用于标识不同的生产者（Producer）和消费者（Consumer）类型。
	*  每种类型对应不同的媒体传输协议。
	*  
	*  生产者类型说明：
	*  - kProducerTypeGB28181: GB28181协议生产者，用于接收GB28181设备的媒体流
	*  - kProducerTypeRtc: RTC协议生产者，用于接收WebRTC推流的媒体流
	*  - kProducerTypeRtmp: RTMP协议生产者，用于接收RTMP推流的媒体流
	*  - kProducerTypeRtsp: RTSP协议生产者，用于接收RTSP推流的媒体流
	*  
	*  消费者类型说明：
	*  - kConsumerTypeRTC: RTC协议消费者，用于WebRTC拉流播放
	*  - kConsumerTypeFlv: FLV协议消费者，用于HTTP-FLV拉流播放
	*  - kConsumerTypeRtmp: RTMP协议消费者，用于RTMP拉流播放
	*  - kConsumerTypeRtsp: RTSP协议消费者，用于RTSP拉流播放
	*  
	*  @note kShareResourceTypeUnknowed表示未知类型，用于错误处理
	*/
	enum  ShareResourceType
	{
	 
		kProducerTypeGB28181 = 0,      ///< GB28181协议生产者
		kProducerTypeRtc=1,            ///< RTC协议生产者
		kProducerTypeRtmp,             ///< RTMP协议生产者
		kProducerTypeRtsp,             ///< RTSP协议生产者
		//  
		kConsumerTypeRTC,              ///< RTC协议消费者
		kConsumerTypeFlv,              ///< FLV协议消费者
		kConsumerTypeRtmp,             ///< RTMP协议消费者
		kConsumerTypeRtsp,             ///< RTSP协议消费者
		kShareResourceTypeUnknowed = 255,  ///< 未知类型

	};
	class Stream;
	class Session;

	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 共享资源基类（Share Resource Base Class）
	*  
	*  ShareResource是GBMediaServer流媒体服务器中所有生产者和消费者的基类。
	*  它提供了共同的功能，包括应用名称、流名称、参数管理、远程地址管理等。
	*  Producer和Consumer类都继承自此类。
	*  
	*  ShareResource功能：
	*  1. 管理应用名称（App Name）和流名称（Stream Name）
	*  2. 管理附加参数（Param）
	*  3. 管理远程地址（Remote Address）
	*  4. 提供流对象（Stream）和会话对象（Session）的访问
	*  5. 提供数据接收和处理的虚函数接口
	*  6. 提供资源类型标识
	*  
	*  应用和流名称说明：
	*  - 格式通常为 "app/stream"，如 "live/stream1"
	*  - app_name_: 应用名称，如 "live"
	*  - stream_name_: 流名称，如 "stream1"
	*  - param_: 附加参数，用于传递协议特定的配置信息
	*  
	*  @note ShareResource是抽象基类，不能直接实例化
	*  @note 子类需要实现纯虚函数，如OnRecv、OnDataChannel等
	*  @note 使用enable_shared_from_this支持智能指针管理
	*  
	*  继承层次：
	*  - Producer: 继承自ShareResource，用于接收媒体流
	*  - Consumer: 继承自ShareResource，用于发送媒体流
	*  
	*  使用示例：
	*  @code
	*  // ShareResource不能直接实例化，通过子类使用
	*  auto producer = std::make_shared<RtcProducer>(stream, session);
	*  producer->SetAppName("live");
	*  producer->SetStreamName("stream1");
	*  @endcode
	*/
	class ShareResource : public std::enable_shared_from_this<ShareResource>
	{
	public:
		explicit ShareResource(const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		virtual ~ShareResource();

	public:
		const std::string & AppName() const;
		void SetAppName(const std::string & app_name);
		const std::string & StreamName() const;
		void SetStreamName(const std::string & stream);
		const std::string & Param() const;
		void SetParam(const std::string & param);




		//接受不同协议上层处理 实现
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}


		virtual void OnDataChannel(const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, uint32_t ppid, const uint8_t* msg, size_t len) {}

		virtual ShareResourceType ShareResouceType() const { return kShareResourceTypeUnknowed; }
		 
		virtual const rtc::SocketAddress &RemoteAddress() const
		{
			return remote_address_;
		}
		virtual void  SetRemoteAddress(const rtc::SocketAddress & addr);

		std::shared_ptr<Session> GetSession() const
		{
			return session_;
		}
		std::shared_ptr < Stream> GetStream() const
		{
			return stream_;
		}
		std::shared_ptr < Stream> GetStream()
		{
			return stream_;
		}
	protected:
		std::shared_ptr < Stream> stream_;
		std::string     app_name_;
		std::string     stream_name_;
		std::string     param_; 
		rtc::SocketAddress   remote_address_;
		int64_t			start_timestamp_{ 0 }; // 启始时间 
		 
		std::shared_ptr < Session> session_;
	};
}

#endif // 