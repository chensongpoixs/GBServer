

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
 
#include "rtc_base/time_utils.h" 
#include "server/session.h" 
#include "consumer/consumer.h"
#include "server/stream.h"
#include "common_video/h264/h264_common.h"
namespace gb_media_server
{ 
	Consumer::Consumer(  const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s)
			:ShareResource(stream, s)
			, sps_()
			, pps_()
			, send_sps_pps_(false)
		, video_buffer_frame_(1024 * 1024 * 8)
		{

			 
		}

	void Consumer::AddVideoFrame(const libmedia_codec::EncodedImage & frame)
	{
		std::vector<webrtc::H264::NaluIndex> nalus = webrtc::H264::FindNaluIndices(
			frame.data(), frame.size());
		size_t fragments_count = nalus.size(); 
		video_buffer_frame_.SetSize(0); 
		for (int32_t nal_index = 0; nal_index < fragments_count; ++nal_index)
		{ 
			webrtc::NaluInfo nalu;
			nalu.type = frame.data()[nalus[nal_index].payload_start_offset] & 0x1F;
			nalu.sps_id = -1;
			nalu.pps_id = -1; 
			switch (nalu.type) {
			case webrtc::H264::NaluType::kSps: {
				//  GBMEDIASERVER_LOG_T_F(LS_INFO) << "======"<<nal_index <<"============>SPS, size:" << nalus[nal_index].payload_size;
				  
					sps_ = rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4);
				 

				break;
			}
			case webrtc::H264::NaluType::kPps: {
				//GBMEDIASERVER_LOG_T_F(LS_INFO) << "=========" << nal_index << "=========>PPS, size:" << nalus[nal_index].payload_size;
				 
					pps_ = rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4);;

				 
				break;
			}
			case webrtc::H264::NaluType::kIdr:
			{
				//GBMEDIASERVER_LOG_T_F(LS_INFO) << "=======" << nal_index << "===========>IDR, size:" << nalus[nal_index].payload_size;
				//if (!send_sps_pps_)
				{ // sps有变在发送sps和pps的信息
					video_buffer_frame_.AppendData(sps_);
					video_buffer_frame_.AppendData(pps_);
				}
				video_buffer_frame_.AppendData(rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4));
				video_buffer_frame_.SetSize(  sps_.size() + pps_.size() + nalus[nal_index].payload_size + 4);
				 
				break;
			}
			case webrtc::H264::NaluType::kSlice: {
				//GBMEDIASERVER_LOG_T_F(LS_INFO) << "======" << nal_index << "============>kSlice, size:" << nalus[nal_index].payload_size;
				video_buffer_frame_.AppendData(frame);
				video_buffer_frame_.SetSize( frame.size());
				break;
			}
												 // Slices below don't contain SPS or PPS ids.
			case webrtc::H264::NaluType::kAud:
			case webrtc::H264::NaluType::kEndOfSequence:
			case webrtc::H264::NaluType::kEndOfStream:
			case webrtc::H264::NaluType::kFiller:
			case webrtc::H264::NaluType::kSei:
			{
				// GBMEDIASERVER_LOG_T_F(LS_INFO) << "=======" << nal_index << "===========>kAud, size:" << nalus[nal_index].payload_size;
				video_buffer_frame_.AppendData(frame);
				video_buffer_frame_.SetSize(  frame.size());
				break;
			}
			case webrtc::H264::NaluType::kStapA:
			case webrtc::H264::NaluType::kFuA:
			{

				// GBMEDIASERVER_LOG_T_F(LS_INFO) << "========" << nal_index << "==========>kFuA---kStapA  , size:" << nalus[nal_index].payload_size;
				video_buffer_frame_.AppendData(frame);
				video_buffer_frame_.SetSize(  frame.size());
				break;
			}
			default: {
				// GBMEDIASERVER_LOG_T_F(LS_INFO) << "=====" << nal_index << "=============>default  packet  , size:" << nalus[nal_index].payload_size;
				video_buffer_frame_.AppendData(frame);
				video_buffer_frame_.SetSize(  frame.size());
				break;
			}

			}
		}
		 
		if (video_buffer_frame_.size() <= 0)
		{
			return;
		}
		send_sps_pps_ = true;
		libmedia_codec::EncodedImage   enocded_image;
		enocded_image.SetTimestamp(frame.Timestamp());
		enocded_image.SetEncodedData(libmedia_codec::EncodedImageBuffer::Create(
			video_buffer_frame_.data(),
			video_buffer_frame_.size()
		)); 
		OnVideoFrame(std::move(enocded_image));
	}

	void Consumer::AddAudioFrame(const rtc::CopyOnWriteBuffer & frame)
	{
		OnAudioFrame(frame);
	}
		 
		 
		 
	 
}