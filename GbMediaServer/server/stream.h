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

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR


实时流

1. 一路实时输入的艺术品数据流
2. 服务器被动接收推流
3. 服务器自动拉流




- 处理输入音视频数据
	1. 修正时间戳， 处理index
	2. 处理CodecHeader
	3. 处理GOP
	4. 保存音视频帧

- 输出音视频数据



1. 定位GOP， 查找CodecHeader
2. 必须先发送CodecHeader， 再发送音视频数据
3. 跳帧，重新查找CodecHeader
4. 输出一定数量的音视频帧






										   流程图


			  请求音视频帧



			已经输出音视频序列头                     否定 ===>          |          定位GOP
																	|
																	|
				  是												    |
																	|
			音视频帧落后太多?                       是   ===>          |          定位GOP

				  否定                                                           ||

																			查找音视频序列头

																				 ||

																			输出音视频序列头


			输出一定数量的音视频帧




流超时


- 长时间没有收到实时数据， 记录收到数据的时间 当前时间收到数据的时间差， 超过一定的时间， 流就算超时
- 流超时是被动检测




流准备好

- 收到关键帧


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
************************************************************************************************/

#ifndef _C_GB_MEDIA_SERVER_STREAM_H_
#define _C_GB_MEDIA_SERVER_STREAM_H_



#include <cstdint>
#include <memory>
#include <string>
#include <atomic>
#include <mutex>
#include "rtc_base/copy_on_write_buffer.h"
#include "libmedia_codec/encoded_frame.h"
#include "libmedia_codec/encoded_image.h"
//#include "server/session.h"
#include "libmedia_transfer_protocol/libhls/chls_muxer.h"
namespace gb_media_server
{
	 
		  
		class Session;
		
		/***
		 *  @author chensong
		 *  @date 2025-10-18
		 *  @brief 媒体流管理类（Stream）
		 *  
		 *  该类是媒体服务器的核心流管理类，负责处理和分发音视频数据流。
		 *  实现了实时流的接收、处理、缓存和分发功能，支持多种流媒体协议。
		 *  
		 *  主要功能：
		 *  1. 音视频帧接收 - 接收来自生产者的编码后的音视频帧
		 *  2. 时间戳处理 - 修正和同步音视频时间戳
		 *  3. GOP管理 - 管理视频GOP（Group of Pictures）缓存
		 *  4. 编解码器头部处理 - 处理和缓存SPS/PPS等编解码器配置信息
		 *  5. HLS支持 - 生成M3U8播放列表和TS切片
		 *  6. 帧分发 - 将音视频帧分发给所有消费者
		 *  
		 *  流处理流程：
		 *  1. 生产者推送音视频帧到Stream
		 *  2. Stream修正时间戳并缓存关键帧
		 *  3. 检测到关键帧后，流状态变为"准备好"
		 *  4. 消费者请求数据时，先发送编解码器头部
		 *  5. 然后按顺序发送音视频帧
		 *  6. 如果消费者落后太多，重新定位到最新的GOP
		 *  
		 *  流超时机制：
		 *  - 记录最后接收数据的时间
		 *  - 如果长时间未收到数据，流标记为超时
		 *  - 超时的流会被自动清理
		 *  
		 *  流准备状态：
		 *  - 收到第一个关键帧后，流进入"准备好"状态
		 *  - 只有准备好的流才能被消费者播放
		 *  
		 *  @note 该类使用enable_shared_from_this，支持安全的shared_ptr管理
		 *  @note 所有音视频帧操作都使用移动语义，避免数据拷贝
		 *  @note 支持HLS协议，自动生成M3U8和TS切片
		 */
		class Stream : public std::enable_shared_from_this<Stream>
		{
		public:
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 构造函数
			 *  
			 *  初始化Stream实例，关联到指定的会话。
			 *  
			 *  @param s 所属的会话引用
			 *  @param session_name 会话名称，用于标识流
			 *  @note 使用explicit防止隐式类型转换
			 *  @note 会话引用在Stream生命周期内必须有效
			 */
			explicit Stream( Session &s, const std::string & session_name);



		public:

			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 添加视频帧（Add Video Frame）
			 *  
			 *  接收并处理来自生产者的编码后的视频帧。
			 *  
			 *  处理流程：
			 *  1. 接收编码后的视频帧（H264/H265等）
			 *  2. 检查是否为关键帧（IDR帧）
			 *  3. 如果是关键帧，更新GOP缓存
			 *  4. 修正视频帧时间戳
			 *  5. 将视频帧传递给HLS Muxer（如果启用）
			 *  6. 将视频帧分发给会话中的所有消费者
			 *  
			 *  关键帧处理：
			 *  - 关键帧标记流进入"准备好"状态
			 *  - 关键帧作为GOP的起始点
			 *  - 新消费者必须从关键帧开始播放
			 *  
			 *  @param frame 编码后的视频帧，使用移动语义避免拷贝
			 *  @note 该方法会在生产者线程中调用
			 *  @note 视频帧包含编码数据、时间戳、帧类型等信息
			 *  @note 使用std::move传递帧数据，调用后frame将失效
			 *  @note TODO: 需要完善HLS协议的实现
			 */
			void AddVideoFrame(  libmedia_codec::EncodedImage &&frame);
			
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 添加音频帧（Add Audio Frame）
			 *  
			 *  接收并处理来自生产者的编码后的音频帧。
			 *  
			 *  处理流程：
			 *  1. 接收编码后的音频帧（AAC/Opus等）
			 *  2. 修正音频帧时间戳
			 *  3. 将音频帧传递给HLS Muxer（如果启用）
			 *  4. 将音频帧分发给会话中的所有消费者
			 *  
			 *  音频帧特点：
			 *  - 音频帧通常较小且频繁
			 *  - 音频帧没有关键帧概念
			 *  - 音频时间戳用于音视频同步
			 *  
			 *  @param frame 编码后的音频帧数据，使用移动语义避免拷贝
			 *  @param pts 音频帧的显示时间戳（Presentation Timestamp）
			 *  @note 该方法会在生产者线程中调用
			 *  @note 音频帧的pts用于与视频帧同步
			 *  @note 使用std::move传递帧数据，调用后frame将失效
			 *  @note TODO: 需要完善HLS协议的实现
			 */
			void AddAudioFrame(  rtc::CopyOnWriteBuffer&& frame, int64_t  pts);
	 
		public:
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 获取HLS播放列表（Get Play List）
			 *  
			 *  返回HLS协议的M3U8播放列表内容。
			 *  
			 *  M3U8播放列表包含：
			 *  - 播放列表版本信息
			 *  - 目标切片时长
			 *  - TS切片文件列表
			 *  - 每个切片的时长信息
			 *  
			 *  @return std::string M3U8播放列表的文本内容
			 *  @note 播放列表由HLS Muxer动态生成
			 *  @note 客户端会定期请求播放列表以获取最新切片
			 *  @note 播放列表采用UTF-8编码
			 */
			std::string  GetPlayList()
			{
				return hls_muxer_.PlayList();
			}
			
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 获取HLS TS切片（Get Fragment）
			 *  
			 *  根据切片名称获取对应的TS切片数据。
			 *  
			 *  切片管理：
			 *  - 切片由HLS Muxer生成和管理
			 *  - 每个切片包含固定时长的音视频数据
			 *  - 切片数据缓存在内存中
			 *  - 旧切片会被自动清理
			 *  
			 *  @param name TS切片文件名（如 "stream-0.ts"）
			 *  @return std::shared_ptr<Fragment> TS切片对象的智能指针，如果不存在返回nullptr
			 *  @note 切片名称必须与M3U8播放列表中的名称一致
			 *  @note Fragment对象包含切片的二进制数据和大小信息
			 *  @note 使用智能指针管理切片生命周期
			 */
			std::shared_ptr< libmedia_transfer_protocol::libhls::Fragment>  GetFragement(const std::string &name)
			{
				return hls_muxer_.GetFragment(name);
			}


		public:

			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 获取会话名称（Session Name）
			 *  
			 *  返回该流所属的会话名称。
			 *  
			 *  会话名称用途：
			 *  - 唯一标识一个媒体流
			 *  - 用于日志记录和调试
			 *  - 用于流的查找和管理
			 *  
			 *  @return const std::string& 会话名称的常量引用
			 *  @note 会话名称在Stream创建时设置，之后不可更改
			 *  @note 返回引用避免字符串拷贝
			 */
			const std::string & SessionName() const;
		
 


		private:
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 所属会话引用（Session Reference）
			 *  
			 *  指向该流所属的会话对象的引用。
			 *  
			 *  用途：
			 *  - 将音视频帧分发给会话中的所有消费者
			 *  - 访问会话级别的配置和状态
			 *  - 与会话进行双向通信
			 *  
			 *  @note 使用引用而非指针，确保会话对象始终有效
			 *  @note 会话对象的生命周期必须长于Stream对象
			 */
			 Session  &session_;
			 
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief 会话名称（Session Name）
			 *  
			 *  该流的唯一标识符，用于区分不同的媒体流。
			 *  
			 *  命名规则：
			 *  - 通常格式为 "app/stream"
			 *  - 例如：live/stream1, vod/movie1
			 *  - 必须在整个服务器中唯一
			 *  
			 *  @note 会话名称在构造时设置，之后不可更改
			 *  @note 用于日志记录、流查找和管理
			 */
			std::string    session_name_;
			 
			/***
			 *  @author chensong
			 *  @date 2025-10-18
			 *  @brief HLS复用器（HLS Muxer）
			 *  
			 *  负责将音视频帧封装为HLS格式，生成M3U8播放列表和TS切片。
			 *  
			 *  主要功能：
			 *  - 接收音视频帧并封装为TS格式
			 *  - 按时长切分TS切片（通常5-10秒）
			 *  - 生成和更新M3U8播放列表
			 *  - 管理TS切片的缓存和清理
			 *  
			 *  HLS工作流程：
			 *  1. 接收音视频帧
			 *  2. 封装为MPEG-TS格式
			 *  3. 达到切片时长后生成新切片
			 *  4. 更新M3U8播放列表
			 *  5. 清理过期的旧切片
			 *  
			 *  @note HLS Muxer在Stream构造时创建
			 *  @note 使用会话名称作为HLS流的标识
			 *  @note TODO: 当前HLS功能尚未完全实现
			 */
			libmedia_transfer_protocol::libhls::HLSMuxer      hls_muxer_;



		};
	 
}


#endif 