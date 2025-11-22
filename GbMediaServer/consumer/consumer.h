

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR






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

#ifndef _C_GB_MEDIA_SERVER_CONSUMER_H_
#define _C_GB_MEDIA_SERVER_CONSUMER_H_



#include <cstdint>
#include <memory>   
#include <vector>  
#include <string>

#include <atomic>

#include "rtc_base/copy_on_write_buffer.h"

#include "libmedia_codec/encoded_image.h"
#include "rtc_base/socket_address.h"
//#include "server/stream.h"
//#include "server/session.h"
#include "share/share_resource.h"

#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 
#include "libmedia_transfer_protocol/muxer/muxer.h"
#include "libmedia_transfer_protocol/librtc/sctp_association.h"
namespace gb_media_server
{
	 

		 
		 
		
		 
		 
		class Consumer : public ShareResource, public sigslot::has_slots<>
		{
		public:
			Consumer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
			virtual ~Consumer() ;
		//public:
		public:
			 

			// 本地采集的数据进行编码后进行发送的接口
			void   SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> f);
			void   SendAudioEncode(std::shared_ptr<libmedia_codec::AudioEncoder::EncodedInfoLeaf> f);
		public:

			//接受不同协议上层处理 实现
			//virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}

			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 添加视频帧（Add Video Frame）
			*  
			*  该方法用于向消费者添加视频帧。在添加视频帧时，会解析H264编码的NALU单元，
			*  提取SPS（Sequence Parameter Set）和PPS（Picture Parameter Set），并在需要时
			*  将它们附加到视频帧前，以确保解码器能够正确解码视频。
			*  
			*  视频帧处理流程：
			*  1. 解析H264编码数据，识别NALU单元类型（SPS、PPS、IDR、Slice等）
			*  2. 提取并缓存SPS和PPS信息
			*  3. 当遇到IDR帧时，在帧前附加SPS和PPS
			*  4. 将处理后的视频帧传递给 OnVideoFrame 方法进行实际发送
			*  
			*  NALU类型处理：
			*  - SPS（Sequence Parameter Set）: 提取并缓存，用于后续帧解码
			*  - PPS（Picture Parameter Set）: 提取并缓存，用于后续帧解码
			*  - IDR帧: 在帧前附加SPS和PPS，确保解码器能够快速恢复
			*  - 普通Slice帧: 直接添加到视频缓冲区
			*  - 其他NALU类型（AUD、SEI等）: 根据需要进行处理
			*  
			*  @param frame 编码后的视频帧，包含H264编码数据和元信息（时间戳、尺寸等）
			*  @note 该方法会解析H264 NALU单元，并在IDR帧前自动附加SPS和PPS
			*  @note 该方法会调用 OnVideoFrame 方法将处理后的帧传递给具体的消费者实现
			*  
			*  使用示例：
			*  @code
			*  libmedia_codec::EncodedImage frame;
			*  frame.SetTimestamp(timestamp);
			*  frame.SetEncodedData(encoded_data);
			*  consumer->AddVideoFrame(frame);
			*  @endcode
			*/
			void     AddVideoFrame(const libmedia_codec::EncodedImage &frame);

			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 添加音频帧（Add Audio Frame）
			*  
			*  该方法用于向消费者添加音频帧。音频帧的处理相对简单，直接将音频数据
			*  和时间戳传递给 OnAudioFrame 方法进行实际发送，无需额外的解码参数。
			*  
			*  音频帧处理流程：
			*  1. 接收音频编码数据和PTS（Presentation Time Stamp，呈现时间戳）
			*  2. 将音频帧直接传递给 OnAudioFrame 方法
			*  3. 由具体的消费者实现类（如RtcConsumer、FlvConsumer等）处理音频帧的封装和发送
			*  
			*  音频数据说明：
			*  - 音频帧通常包含一个或多个音频采样周期的编码数据
			*  - 音频编码格式可能为AAC、OPUS等，由具体的消费者实现决定
			*  - PTS用于同步音频和视频的播放时间
			*  
			*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据
			*  @param pts 呈现时间戳（Presentation Time Stamp），单位通常为毫秒或采样周期
			*  @note 音频帧处理不需要像视频帧那样附加SPS/PPS等参数
			*  @note 该方法会调用 OnAudioFrame 方法将音频帧传递给具体的消费者实现
			*  
			*  使用示例：
			*  @code
			*  rtc::CopyOnWriteBuffer audio_data(aac_encoded_data, data_size);
			*  int64_t pts = rtc::TimeMillis(); // 或使用音频采样时间戳
			*  consumer->AddAudioFrame(audio_data, pts);
			*  @endcode
			*/
			void     AddAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);
			
			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 处理视频帧（On Video Frame）
			*  
			*  该方法是消费者处理视频帧的虚函数，由具体的消费者实现类（如RtcConsumer、
			*  FlvConsumer、RtspConsumer等）重写，用于将视频帧封装为特定协议格式并发送。
			*  
			*  该方法在 AddVideoFrame 中被调用，接收已经处理过的视频帧（包含SPS/PPS）。
			*  具体的消费者实现类需要：
			*  1. 将视频帧封装为对应的协议格式（RTP、FLV、RTSP等）
			*  2. 设置正确的协议头部信息（时间戳、序列号等）
			*  3. 通过网络连接发送封装后的数据
			*  
			*  @param frame 编码后的视频帧，已经包含必要的SPS和PPS信息
			*  @note 该方法是虚函数，默认实现为空，由子类重写实现具体的视频帧发送逻辑
			*  @note 不同的消费者实现类会以不同的方式处理视频帧（RTP打包、FLV封装等）
			*  
			*  实现示例（RtcConsumer）：
			*  - 将视频帧封装为RTP包
			*  - 设置RTP头部信息（SSRC、时间戳、序列号等）
			*  - 通过SRTP加密后发送
			*  
			*  实现示例（FlvConsumer）：
			*  - 将视频帧封装为FLV视频标签
			*  - 添加FLV头部信息
			*  - 通过HTTP连接发送
			*/
			virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame) {}

			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 处理音频帧（On Audio Frame）
			*  
			*  该方法是消费者处理音频帧的虚函数，由具体的消费者实现类（如RtcConsumer、
			*  FlvConsumer、RtspConsumer等）重写，用于将音频帧封装为特定协议格式并发送。
			*  
			*  该方法在 AddAudioFrame 中被调用，接收音频编码数据和PTS时间戳。
			*  具体的消费者实现类需要：
			*  1. 将音频帧封装为对应的协议格式（RTP、FLV、RTSP等）
			*  2. 设置正确的协议头部信息（时间戳、采样率等）
			*  3. 通过网络连接发送封装后的数据
			*  
			*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据
			*  @param pts 呈现时间戳（Presentation Time Stamp），用于音频和视频同步
			*  @note 该方法是虚函数，默认实现为空，由子类重写实现具体的音频帧发送逻辑
			*  @note 不同的消费者实现类会以不同的方式处理音频帧（RTP打包、FLV封装等）
			*  
			*  实现示例（RtcConsumer）：
			*  - 将音频帧封装为RTP包
			*  - 设置RTP头部信息（SSRC、时间戳、序列号等）
			*  - 通过SRTP加密后发送
			*  
			*  实现示例（FlvConsumer）：
			*  - 将音频帧封装为FLV音频标签
			*  - 添加FLV头部信息
			*  - 通过HTTP连接发送
			*/
			virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts) {}

			
		public:
			//网络层接口
			//void Close();
			
		protected:
			rtc::CopyOnWriteBuffer                     sps_;
			rtc::CopyOnWriteBuffer						pps_;

			bool                                         send_sps_pps_;
			rtc::Buffer                                video_buffer_frame_;


			libmedia_transfer_protocol::Muxer    *      muxer_;
		};
 
}

#endif // 