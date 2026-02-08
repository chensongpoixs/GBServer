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

#include "share/share_resource.h"
#include "rtc_base/time_utils.h" 
#include "server/session.h" 
#include "consumer/consumer.h"
#include "server/stream.h"
namespace  gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 构造共享资源对象（Constructor）
	*  
	*  该构造函数用于初始化共享资源对象。共享资源是生产者和消费者的基类，
	*  提供了流对象和会话对象的访问接口。
	*  
	*  初始化流程：
	*  1. 保存流对象的共享指针，用于访问媒体流数据
	*  2. 保存会话对象的共享指针，用于管理会话状态
	*  3. 记录资源创建的时间戳，用于统计资源使用时长
	*  
	*  @param stream 流对象的共享指针，用于访问媒体流数据，不能为空
	*  @param s 会话对象的共享指针，用于管理会话状态，不能为空
	*  @note 流对象和会话对象通过共享指针管理，确保生命周期正确
	*  @note 创建时间戳用于统计资源使用时长和性能分析
	*/
	ShareResource::ShareResource(const std::shared_ptr<Stream>& stream, const std::shared_ptr<Session>& s)
		: stream_(stream), session_(s)
	{
		start_timestamp_ = rtc::TimeMillis();
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 析构共享资源对象（Destructor）
	*  
	*  该析构函数用于清理共享资源对象。由于使用共享指针管理流对象和会话对象，
	*  不需要手动释放资源。
	*/
	ShareResource::~ShareResource()
	{
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 获取应用名称（Get Application Name）
	*  
	*  该方法用于获取流媒体应用的名称。应用名称是流媒体URL的一部分，
	*  用于标识不同的应用或业务场景。
	*  
	*  @return 返回应用名称的常量引用
	*  @note 应用名称通常在URL中指定，例如"live"、"vod"等
	*/
	const std::string & ShareResource::AppName() const
	{
		// TODO: insert return statement here
		return app_name_;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 设置应用名称（Set Application Name）
	*  
	*  该方法用于设置流媒体应用的名称。应用名称是流媒体URL的一部分，
	*  用于标识不同的应用或业务场景。
	*  
	*  @param app_name 应用名称，例如"live"、"vod"等
	*/
	void ShareResource::SetAppName(const std::string & app_name)
	{
		app_name_ = app_name;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 获取流名称（Get Stream Name）
	*  
	*  该方法用于获取流媒体流的名称。流名称是流媒体URL的一部分，
	*  用于唯一标识一个媒体流。
	*  
	*  @return 返回流名称的常量引用
	*  @note 流名称通常在URL中指定，例如"stream1"、"test"等
	*/
	const std::string & ShareResource::StreamName() const
	{
		// TODO: insert return statement here
		return stream_name_;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 设置流名称（Set Stream Name）
	*  
	*  该方法用于设置流媒体流的名称。流名称是流媒体URL的一部分，
	*  用于唯一标识一个媒体流。
	*  
	*  @param stream 流名称，例如"stream1"、"test"等
	*/
	void ShareResource::SetStreamName(const std::string & stream)
	{
		stream_name_ = stream;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 获取URL参数（Get URL Parameters）
	*  
	*  该方法用于获取流媒体URL中的查询参数。URL参数用于传递额外的配置信息，
	*  例如认证令牌、播放参数等。
	*  
	*  @return 返回URL参数的常量引用
	*  @note URL参数通常以"?"开头，例如"?token=abc123&quality=hd"
	*/
	const std::string & ShareResource::Param() const
	{
		// TODO: insert return statement here
		return param_;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 设置URL参数（Set URL Parameters）
	*  
	*  该方法用于设置流媒体URL中的查询参数。URL参数用于传递额外的配置信息，
	*  例如认证令牌、播放参数等。
	*  
	*  @param param URL参数字符串，例如"?token=abc123&quality=hd"
	*/
	void ShareResource::SetParam(const std::string & param)
	{
		param_ = param;
	}
	
	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief 设置远程地址（Set Remote Address）
	*  
	*  该方法用于设置客户端的远程地址（IP和端口）。远程地址用于标识客户端，
	*  并用于发送数据包到客户端。
	*  
	*  @param addr 客户端的套接字地址，包含IP地址和端口号
	*  @note 远程地址在连接建立时设置，用于后续的数据发送
	*/
	void ShareResource::SetRemoteAddress(const rtc::SocketAddress & addr)
	{
		remote_address_ = addr;
	}
}

