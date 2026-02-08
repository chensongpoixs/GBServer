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
	*  每种类型对应不同的媒体传输协议。在GBMediaServer流媒体服务器中，
	*  生产者负责接收媒体流，消费者负责发送媒体流。
	*  
	*  生产者类型说明：
	*  - kProducerTypeGB28181: GB28181协议生产者，用于接收GB28181设备的媒体流
	*    GB28181是中国公共安全视频监控联网标准，广泛应用于安防监控领域
	*  - kProducerTypeRtc: RTC协议生产者，用于接收WebRTC推流的媒体流
	*    WebRTC是实时通信协议，支持低延迟的音视频传输
	*  - kProducerTypeRtmp: RTMP协议生产者，用于接收RTMP推流的媒体流
	*    RTMP是Adobe开发的流媒体协议，广泛用于直播推流
	*  - kProducerTypeRtsp: RTSP协议生产者，用于接收RTSP推流的媒体流
	*    RTSP是实时流协议，常用于IP摄像头和监控设备
	*  
	*  消费者类型说明：
	*  - kConsumerTypeRTC: RTC协议消费者，用于WebRTC拉流播放
	*    支持浏览器和移动端的实时播放
	*  - kConsumerTypeFlv: FLV协议消费者，用于HTTP-FLV拉流播放
	*    HTTP-FLV延迟低，适合直播场景
	*  - kConsumerTypeRtmp: RTMP协议消费者，用于RTMP拉流播放
	*    兼容Flash播放器和RTMP客户端
	*  - kConsumerTypeRtsp: RTSP协议消费者，用于RTSP拉流播放
	*    适合监控客户端和专业播放器
	*  
	*  @note kShareResourceTypeUnknowed表示未知类型，用于错误处理
	*  @note 枚举值从0开始，便于数组索引和统计
	*  
	*  使用示例：
	*  @code
	*  ShareResourceType type = producer->ShareResouceType();
	*  if (type == kProducerTypeRtc) {
	*      std::cout << "This is a WebRTC producer" << std::endl;
	*  }
	*  @endcode
	*/
	enum  ShareResourceType
	{
	 
		kProducerTypeGB28181 = 0,      ///< GB28181协议生产者，用于接收国标设备推流
		kProducerTypeRtc=1,            ///< RTC协议生产者，用于接收WebRTC推流
		kProducerTypeRtmp,             ///< RTMP协议生产者，用于接收RTMP推流
		kProducerTypeRtsp,             ///< RTSP协议生产者，用于接收RTSP推流
		//  
		kConsumerTypeRTC,              ///< RTC协议消费者，用于WebRTC拉流播放
		kConsumerTypeFlv,              ///< FLV协议消费者，用于HTTP-FLV拉流播放
		kConsumerTypeRtmp,             ///< RTMP协议消费者，用于RTMP拉流播放
		kConsumerTypeRtsp,             ///< RTSP协议消费者，用于RTSP拉流播放
		kShareResourceTypeUnknowed = 255,  ///< 未知类型，用于错误处理

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
	*  Producer和Consumer类都继承自此类，实现了流媒体服务器的核心抽象。
	*  
	*  ShareResource功能：
	*  1. 管理应用名称（App Name）和流名称（Stream Name）
	*     - 应用名称用于区分不同的业务场景（如live、vod等）
	*     - 流名称用于唯一标识一个媒体流
	*  2. 管理附加参数（Param）
	*     - 用于传递协议特定的配置信息
	*     - 如认证token、编码参数等
	*  3. 管理远程地址（Remote Address）
	*     - 存储客户端的IP地址和端口
	*     - 用于发送数据包和连接管理
	*  4. 提供流对象（Stream）和会话对象（Session）的访问
	*     - Stream管理媒体流的生命周期
	*     - Session管理客户端会话
	*  5. 提供数据接收和处理的虚函数接口
	*     - OnRecv: 接收网络数据
	*     - OnDataChannel: 接收数据通道消息
	*  6. 提供资源类型标识
	*     - 用于运行时类型识别
	*  
	*  应用和流名称说明：
	*  - 格式通常为 "app/stream"，如 "live/stream1"
	*  - app_name_: 应用名称，如 "live"（直播）、"vod"（点播）
	*  - stream_name_: 流名称，如 "stream1"、"camera001"
	*  - param_: 附加参数，如 "token=abc123&quality=high"
	*  
	*  @note ShareResource是抽象基类，不能直接实例化
	*  @note 子类需要实现纯虚函数，如ShareResouceType()
	*  @note 使用enable_shared_from_this支持智能指针管理，避免循环引用
	*  @note 所有成员变量都是protected，子类可以直接访问
	*  
	*  继承层次：
	*  - Producer: 继承自ShareResource，用于接收媒体流（推流端）
	*  - Consumer: 继承自ShareResource，用于发送媒体流（拉流端）
	*  
	*  使用示例：
	*  @code
	*  // ShareResource不能直接实例化，通过子类使用
	*  auto producer = std::make_shared<RtcProducer>(stream, session);
	*  producer->SetAppName("live");
	*  producer->SetStreamName("stream1");
	*  producer->SetParam("token=abc123");
	*  
	*  // 获取资源类型
	*  ShareResourceType type = producer->ShareResouceType();
	*  @endcode
	*/
	class ShareResource : public std::enable_shared_from_this<ShareResource>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化ShareResource实例。它会保存流对象和会话对象的引用，
		*  并记录创建时间戳。
		*  
		*  初始化流程：
		*  1. 保存流对象的共享指针
		*  2. 保存会话对象的共享指针
		*  3. 记录创建时间戳（毫秒）
		*  
		*  @param stream 流对象的共享指针，用于管理媒体流
		*  @param s 会话对象的共享指针，用于管理客户端会话
		*  @note 流对象和会话对象必须有效，不能为空
		*  @note 创建时间戳用于统计连接时长
		*/
		explicit ShareResource(const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理ShareResource实例。由于使用智能指针管理资源，
		*  不需要手动释放内存。
		*  
		*  @note 析构时会自动释放流对象和会话对象的引用
		*/
		virtual ~ShareResource();

	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取应用名称（Get App Name）
		*  
		*  该方法用于获取应用名称。应用名称用于区分不同的业务场景。
		*  
		*  @return 返回应用名称的常量引用
		*  @note 应用名称通常在创建时设置，如"live"、"vod"等
		*/
		const std::string & AppName() const;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 设置应用名称（Set App Name）
		*  
		*  该方法用于设置应用名称。
		*  
		*  @param app_name 应用名称字符串
		*  @note 应用名称应该在接收到第一个请求时设置
		*/
		void SetAppName(const std::string & app_name);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取流名称（Get Stream Name）
		*  
		*  该方法用于获取流名称。流名称用于唯一标识一个媒体流。
		*  
		*  @return 返回流名称的常量引用
		*  @note 流名称通常在创建时设置，如"stream1"、"camera001"等
		*/
		const std::string & StreamName() const;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 设置流名称（Set Stream Name）
		*  
		*  该方法用于设置流名称。
		*  
		*  @param stream 流名称字符串
		*  @note 流名称应该在接收到第一个请求时设置
		*/
		void SetStreamName(const std::string & stream);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取附加参数（Get Param）
		*  
		*  该方法用于获取附加参数。附加参数用于传递协议特定的配置信息。
		*  
		*  @return 返回附加参数的常量引用
		*  @note 参数格式通常为"key1=value1&key2=value2"
		*/
		const std::string & Param() const;

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 设置附加参数（Set Param）
		*  
		*  该方法用于设置附加参数。
		*  
		*  @param param 附加参数字符串
		*  @note 参数可以包含认证token、编码参数等
		*/
		void SetParam(const std::string & param);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 接收数据（On Receive）
		*  
		*  该方法用于接收来自网络的数据。不同的协议会有不同的实现。
		*  
		*  @param buffer 接收到的数据缓冲区
		*  @note 该方法是虚函数，子类可以重写实现特定协议的数据处理
		*  @note 默认实现为空，不做任何处理
		*/
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 接收数据通道消息（On Data Channel）
		*  
		*  该方法用于接收来自SCTP数据通道的消息。数据通道用于传输非媒体数据。
		*  
		*  @param params SCTP流参数，包含流ID等信息
		*  @param ppid 负载协议标识符（PPID）
		*  @param msg 消息数据指针
		*  @param len 消息长度（字节）
		*  @note 该方法是虚函数，子类可以重写实现数据通道处理
		*  @note 默认实现为空，不做任何处理
		*/
		virtual void OnDataChannel(const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params, uint32_t ppid, const uint8_t* msg, size_t len) {}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取资源类型（Get Share Resource Type）
		*  
		*  该方法用于获取资源的类型标识。用于运行时类型识别。
		*  
		*  @return 返回资源类型枚举值
		*  @note 该方法是虚函数，子类必须重写返回正确的类型
		*  @note 默认返回kShareResourceTypeUnknowed（未知类型）
		*/
		virtual ShareResourceType ShareResouceType() const { return kShareResourceTypeUnknowed; }
		 
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取远程地址（Get Remote Address）
		*  
		*  该方法用于获取远程客户端的网络地址（IP和端口）。
		*  
		*  @return 返回远程地址的常量引用
		*  @note 远程地址在接收到第一个数据包时设置
		*/
		virtual const rtc::SocketAddress &RemoteAddress() const
		{
			return remote_address_;
		}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 设置远程地址（Set Remote Address）
		*  
		*  该方法用于设置远程客户端的网络地址。
		*  
		*  @param addr 远程地址对象
		*  @note 远程地址用于发送数据包
		*/
		virtual void  SetRemoteAddress(const rtc::SocketAddress & addr);

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取会话对象（Get Session）
		*  
		*  该方法用于获取关联的会话对象。
		*  
		*  @return 返回会话对象的共享指针
		*  @note 会话对象管理客户端连接的生命周期
		*/
		std::shared_ptr<Session> GetSession() const
		{
			return session_;
		}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取流对象（Get Stream - const版本）
		*  
		*  该方法用于获取关联的流对象（常量版本）。
		*  
		*  @return 返回流对象的共享指针
		*  @note 流对象管理媒体流的生命周期
		*/
		std::shared_ptr < Stream> GetStream() const
		{
			return stream_;
		}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取流对象（Get Stream - 非const版本）
		*  
		*  该方法用于获取关联的流对象（非常量版本）。
		*  
		*  @return 返回流对象的共享指针
		*  @note 流对象管理媒体流的生命周期
		*/
		std::shared_ptr < Stream> GetStream()
		{
			return stream_;
		}

	protected:
		std::shared_ptr < Stream> stream_;        ///< 流对象，管理媒体流的生命周期
		std::string     app_name_;                ///< 应用名称，如"live"、"vod"
		std::string     stream_name_;             ///< 流名称，如"stream1"、"camera001"
		std::string     param_;                   ///< 附加参数，如"token=abc123"
		rtc::SocketAddress   remote_address_;     ///< 远程客户端地址（IP和端口）
		int64_t			start_timestamp_{ 0 };    ///< 创建时间戳（毫秒），用于统计连接时长
		 
		std::shared_ptr < Session> session_;      ///< 会话对象，管理客户端会话
	};
}

#endif // 