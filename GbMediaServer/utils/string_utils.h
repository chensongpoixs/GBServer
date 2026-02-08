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
				   date:  2025-10-17

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


#ifndef _C_GB_MEDIA_SERVER_STRING_UTILS_H_
#define _C_GB_MEDIA_SERVER_STRING_UTILS_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"

#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"



namespace  gb_media_server
{
	/**
	*  @author chensong
	*  @date 2025-10-17
	*  @brief 字符串工具命名空间（String Utilities Namespace）
	*  
	*  该命名空间提供了一组常用的字符串处理工具函数，用于处理URL、文件路径、字符串分割等操作。
	*  这些工具函数在流媒体服务器中广泛使用，用于解析配置、处理请求、提取信息等。
	*  
	*  主要功能：
	*  1. 字符串分割（split）：按指定分隔符分割字符串
	*  2. URL解析（GetSessionNameFromUrl）：从流媒体URL中提取会话名称
	*  3. 文件扩展名提取（FileExt）：从文件路径中提取扩展名
	*  
	*  使用场景：
	*  - 解析WebRTC、RTMP、RTSP等流媒体URL
	*  - 提取应用名和流名，用于会话管理
	*  - 识别文件类型，用于内容分发
	*  - 处理配置文件和路径
	*  
	*  @note 所有函数都是独立的工具函数，不依赖于类实例
	*  @note 函数设计为线程安全，可以在多线程环境中使用
	*  
	*  使用示例：
	*  @code
	*  using namespace gb_media_server::string_utils;
	*  
	*  // 分割字符串
	*  std::vector<std::string> fields;
	*  split("a,b,c", ',', &fields);
	*  
	*  // 提取会话名称
	*  std::string session = GetSessionNameFromUrl("webrtc://server.com:9091/live/test");
	*  
	*  // 提取文件扩展名
	*  std::string ext = FileExt("/path/to/video.mp4");
	*  @endcode
	*/
	namespace string_utils 
	{
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 字符串分割函数（Split String）
		*  
		*  该函数用于将字符串按照指定的分隔符进行分割，将分割后的子字符串存储到vector中。
		*  这是一个常用的字符串处理工具函数，用于解析配置、URL、路径等。
		*  
		*  @param source 待分割的源字符串
		*  @param delimiter 分隔符字符，用于标识分割位置
		*  @param fields 输出参数，存储分割后的子字符串的vector指针，不能为空
		*  @return 返回分割后的子字符串数量
		*  @note 如果源字符串为空，返回的vector包含一个空字符串
		*  @note 连续的分隔符会产生空字符串
		*  
		*  使用示例：
		*  @code
		*  std::vector<std::string> fields;
		*  size_t count = split("a/b/c", '/', &fields);
		*  // fields = ["a", "b", "c"], count = 3
		*  @endcode
		*/
		size_t split(std::string source,
			char delimiter,
		std::vector<std::string>* fields);

		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 从URL中提取会话名称（Get Session Name From URL）
		*  
		*  该函数用于从WebRTC、RTMP、RTSP等流媒体URL中提取会话名称。
		*  会话名称由应用名（app）和流名（stream）组成，格式为"app/stream"。
		*  
		*  支持的URL格式：
		*  - protocol://domain:port/app/stream
		*  - protocol://domain:port/domain_name/app/stream
		*  
		*  @param url 完整的流媒体URL，包含协议、域名、端口、应用名和流名
		*  @return 返回会话名称（格式为"app/stream"），如果URL格式不正确则返回空字符串
		*  
		*  使用示例：
		*  @code
		*  std::string url = "webrtc://chensong.com:9091/live/test";
		*  std::string session = GetSessionNameFromUrl(url);
		*  // session = "live/test"
		*  @endcode
		*/
		std::string  GetSessionNameFromUrl(const std::string &url);
	
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 提取文件扩展名（File Extension）
		*  
		*  该函数用于从文件路径或文件名中提取文件扩展名（不包含点号）。
		*  扩展名用于识别文件类型，例如"mp4"、"flv"、"txt"等。
		*  
		*  @param path 文件路径或文件名，可以是相对路径或绝对路径
		*  @return 返回文件扩展名（不包含点号），如果没有扩展名则返回文件名
		*  @note 返回的扩展名不包含点号，例如"mp4"而不是".mp4"
		*  
		*  使用示例：
		*  @code
		*  std::string ext = FileExt("/home/user/video.mp4");
		*  // ext = "mp4"
		*  @endcode
		*/
		std::string  FileExt(const std::string & path);
	}
}


#endif // _C_GB_MEDIA_SERVER_STRING_UTILS_H_