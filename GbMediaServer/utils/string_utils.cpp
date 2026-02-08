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

#include "utils/string_utils.h"

#include "rtc_base/string_encode.h"
namespace  gb_media_server
{
	namespace string_utils
	{
		namespace
		{
			/**
			*  @author chensong
			*  @date 2025-10-17
			*  @brief 从路径中提取文件名（包含扩展名）
			*  
			*  该函数用于从完整的文件路径中提取文件名部分（包含扩展名）。
			*  支持Windows和Unix风格的路径分隔符（\ 和 /）。
			*  
			*  处理流程：
			*  1. 从路径末尾向前查找最后一个路径分隔符（/ 或 \）
			*  2. 如果找到分隔符，返回分隔符之后的部分（文件名）
			*  3. 如果没有找到分隔符，说明输入本身就是文件名，直接返回
			*  
			*  @param path 完整的文件路径，可以是相对路径或绝对路径
			*  @return 返回文件名（包含扩展名），如果路径为空或无效则返回原路径
			*  
			*  使用示例：
			*  @code
			*  std::string path1 = "/home/user/test.txt";
			*  std::string filename1 = FileNameExt(path1); // 返回 "test.txt"
			*  
			*  std::string path2 = "C:\\Users\\test\\file.mp4";
			*  std::string filename2 = FileNameExt(path2); // 返回 "file.mp4"
			*  
			*  std::string path3 = "test.txt";
			*  std::string filename3 = FileNameExt(path3); // 返回 "test.txt"
			*  @endcode
			*/
			std::string  FileNameExt(const std::string &path)
			{
				auto pos = path.find_last_of("/\\");
				if (pos != std::string::npos)
				{
					if (pos + 1 < path.size())
					{
						return path.substr(pos + 1);
					}
				}
				return path;
			}
		}
		
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 字符串分割函数（Split String）
		*  
		*  该函数用于将字符串按照指定的分隔符进行分割，将分割后的子字符串存储到vector中。
		*  这是一个常用的字符串处理工具函数，用于解析配置、URL、路径等。
		*  
		*  处理流程：
		*  1. 清空输出vector，确保结果容器为空
		*  2. 遍历源字符串的每个字符
		*  3. 当遇到分隔符时，将上一个分隔符到当前位置之间的子字符串添加到vector
		*  4. 记录当前分隔符的位置，作为下一个子字符串的起始位置
		*  5. 遍历结束后，将最后一个分隔符到字符串末尾的部分添加到vector
		*  
		*  @param source 待分割的源字符串
		*  @param delimiter 分隔符字符，用于标识分割位置
		*  @param fields 输出参数，存储分割后的子字符串的vector指针，不能为空
		*  @return 返回分割后的子字符串数量
		*  @note 如果源字符串为空，返回的vector包含一个空字符串
		*  @note 如果源字符串中没有分隔符，返回的vector包含整个源字符串
		*  @note 连续的分隔符会产生空字符串
		*  
		*  使用示例：
		*  @code
		*  std::vector<std::string> fields;
		*  
		*  // 示例1：分割URL路径
		*  std::string url = "webrtc://server.com:9091/live/stream1";
		*  size_t count = split(url, '/', &fields);
		*  // fields = ["webrtc:", "", "server.com:9091", "live", "stream1"]
		*  // count = 5
		*  
		*  // 示例2：分割逗号分隔的列表
		*  std::string list = "apple,banana,orange";
		*  split(list, ',', &fields);
		*  // fields = ["apple", "banana", "orange"]
		*  
		*  // 示例3：处理连续分隔符
		*  std::string path = "a//b/c";
		*  split(path, '/', &fields);
		*  // fields = ["a", "", "b", "c"]
		*  @endcode
		*/
		size_t split(std::string source,
			char delimiter,
			std::vector<std::string>* fields)
		{
			//RTC_DCHECK(fields);
			fields->clear();
			size_t last = 0;
			for (size_t i = 0; i < source.length(); ++i) {
				if (source[i] == delimiter) {
					fields->emplace_back(source.substr(last, i - last));
					last = i + 1;
				}
			}
			fields->emplace_back(source.substr(last));
			return fields->size();
		}
		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 从URL中提取会话名称（Get Session Name From URL）
		*  
		*  该函数用于从WebRTC、RTMP、RTSP等流媒体URL中提取会话名称。
		*  会话名称由应用名（app）和流名（stream）组成，格式为"app/stream"。
		*  
		*  支持的URL格式：
		*  - 标准格式：protocol://domain:port/app/stream
		*    例如：webrtc://chensong.com:9091/live/test
		*    提取结果：live/test
		*  
		*  - 带域名格式：protocol://domain:port/domain_name/app/stream
		*    例如：webrtc://chensong.com:9091/domain/live/test
		*    提取结果：live/test
		*  
		*  处理流程：
		*  1. 使用split函数按'/'分割URL
		*  2. 根据分割后的段数判断URL格式
		*  3. 提取domain、app和stream部分
		*  4. 从domain中移除端口号（如果存在）
		*  5. 返回"app/stream"格式的会话名称
		*  
		*  @param url 完整的流媒体URL，包含协议、域名、端口、应用名和流名
		*  @return 返回会话名称（格式为"app/stream"），如果URL格式不正确则返回空字符串
		*  @note 该函数会自动处理URL中的端口号，只保留域名部分
		*  @note 如果URL格式不符合预期（段数少于5），返回空字符串
		*  
		*  使用示例：
		*  @code
		*  // 示例1：标准格式URL
		*  std::string url1 = "webrtc://chensong.com:9091/live/test";
		*  std::string session1 = GetSessionNameFromUrl(url1);
		*  // session1 = "live/test"
		*  
		*  // 示例2：带域名格式URL
		*  std::string url2 = "webrtc://chensong.com:9091/domain/live/test";
		*  std::string session2 = GetSessionNameFromUrl(url2);
		*  // session2 = "live/test"
		*  
		*  // 示例3：RTMP URL
		*  std::string url3 = "rtmp://server.com:1935/live/stream1";
		*  std::string session3 = GetSessionNameFromUrl(url3);
		*  // session3 = "live/stream1"
		*  
		*  // 示例4：格式错误的URL
		*  std::string url4 = "invalid/url";
		*  std::string session4 = GetSessionNameFromUrl(url4);
		*  // session4 = ""
		*  @endcode
		*/
		  std::string  GetSessionNameFromUrl(const std::string &url)
		{
			//webrtc://chensong.com:9091/live/test
			//webrtc://chensong.com:9091/domain/live/test 
			std::vector<std::string> list;
			split(url, '/', &list);
			if (list.size() < 5)
			{
				return "";
			}
			std::string domain, app, stream;
			if (list.size() == 5)
			{
				domain = list[2];
				app = list[3];
				stream = list[4];
			}
			else if (list.size() == 6)
			{
				domain = list[3];
				app = list[4];
				stream = list[5];
			}

			auto pos = domain.find_first_of(':');
			if (pos != std::string::npos)
			{
				domain = domain.substr(0, pos);
			}
			  
			return /*domain + "/" + */app + "/" + stream;
		}


		/**
		*  @author chensong
		*  @date 2025-10-17
		*  @brief 提取文件扩展名（File Extension）
		*  
		*  该函数用于从文件路径或文件名中提取文件扩展名（不包含点号）。
		*  扩展名用于识别文件类型，例如"mp4"、"flv"、"txt"等。
		*  
		*  处理流程：
		*  1. 调用FileNameExt函数提取文件名（去除路径部分）
		*  2. 从文件名末尾向前查找最后一个点号（.）
		*  3. 如果找到点号且不在文件名开头，返回点号之后的部分（扩展名）
		*  4. 如果没有找到点号或点号在开头（隐藏文件），返回整个文件名
		*  
		*  @param path 文件路径或文件名，可以是相对路径或绝对路径
		*  @return 返回文件扩展名（不包含点号），如果没有扩展名则返回文件名
		*  @note 返回的扩展名不包含点号，例如"mp4"而不是".mp4"
		*  @note 对于隐藏文件（如".gitignore"），会返回整个文件名
		*  
		*  使用示例：
		*  @code
		*  // 示例1：提取视频文件扩展名
		*  std::string path1 = "/home/user/video.mp4";
		*  std::string ext1 = FileExt(path1);
		*  // ext1 = "mp4"
		*  
		*  // 示例2：提取文本文件扩展名
		*  std::string path2 = "C:\\Users\\test\\document.txt";
		*  std::string ext2 = FileExt(path2);
		*  // ext2 = "txt"
		*  
		*  // 示例3：没有扩展名的文件
		*  std::string path3 = "README";
		*  std::string ext3 = FileExt(path3);
		*  // ext3 = "README"
		*  
		*  // 示例4：隐藏文件
		*  std::string path4 = ".gitignore";
		*  std::string ext4 = FileExt(path4);
		*  // ext4 = ".gitignore"
		*  
		*  // 示例5：多个点号的文件名
		*  std::string path5 = "archive.tar.gz";
		*  std::string ext5 = FileExt(path5);
		*  // ext5 = "gz"（只返回最后一个扩展名）
		*  @endcode
		*/
		  std::string  FileExt(const std::string & path)
		  {
			  std::string filename = FileNameExt(path);
			  auto pos = filename.find_last_of(".");
			  if (pos != std::string::npos)
			  {
				  if (pos != 0)
				  {
					  return filename.substr(pos+1, filename.length()-1);
				  }
			  }
			  return filename;
		  }
	}
}
