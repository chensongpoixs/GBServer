

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		rtmp_header



音视频时间戳矫正工具


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
#include "utils/time_corrector.h"
 
#include <string>
#include <vector>

namespace gb_media_server
{ 
		//uint32_t   TimeCorrector::CorrectTimestamp(const PacketPtr &packet)
		//{
		//
		//	//if (!CodecUtils::IsCodecHeader(packet))
		//	//{
		//	//	// 正常的数据包处理
		//	//	//int32_t pt = packet->PacketType();
		//	//	//if (pt == kPacketTypeVideo)
		//	//	if (packet->IsVideo())
		//	//	{
		//	//		return CorrectVideoTimeStampByVideo(packet);
		//	//	}
		//	//	else if (packet->IsAudio())
		//	//	{
		//	//		return CorrectAudioTimeStampByAudio(packet);
		//	//	}
		//	//}
		//
		//	return 0;
		//
		//}
		//uint32_t   TimeCorrector::CorrectAudioTimeStampByVideo(const PacketPtr& packet)
		//{
		//	++audio_numbers_between_video_;;
		//	//有两个音频包
		//	if (audio_numbers_between_video_ > 1)
		//	{
		//		return CorrectAudioTimeStampByAudio(packet);
		//	}
		//	int64_t time = packet->TimeStamp();
		//	if (video_original_timestamp_ == -1)
		//	{
		//		audio_original_timestamp_ = time;
		//		audio_corrected_timestamp_ = time;
		//		return time;
		//	}
		//
		//	int64_t delta = time - video_original_timestamp_; //原始值
		//	//判断是否在范围之内
		//	bool  fine = (delta > -kMaxVideoDeltaTime) && (delta < kMaxVideoDeltaTime);
		//	if (!fine)
		//	{
		//		delta = kMaxVideoDeltaTime;
		//	}
		//
		//	// 更新时间戳
		//	audio_original_timestamp_ = time;
		//	// 通过视频矫正音频时间戳 
		//	audio_corrected_timestamp_ = video_corrected_timestamp_ + delta;
		//	if (audio_corrected_timestamp_ < 0)
		//	{
		//		audio_corrected_timestamp_ = 0;
		//	}
		//	return audio_corrected_timestamp_;
		//}
		//uint32_t   TimeCorrector::CorrectVideoTimeStampByVideo(const PacketPtr& packet)
		//{
		//	audio_numbers_between_video_ = 0;
		//	int64_t time = packet->TimeStamp();
		//	if (video_original_timestamp_ == -1)
		//	{
		//		video_original_timestamp_ = time;
		//		video_corrected_timestamp_ = time;
		//
		//		// 视频上转呢  音频就先来了 
		//		if (audio_original_timestamp_ != -1)
		//		{
		//			// diff time
		//			int32_t delta = audio_original_timestamp_ - audio_corrected_timestamp_;
		//			if (delta <= -kMaxVideoDeltaTime || delta >= kMaxVideoDeltaTime)
		//			{
		//				// 视频与音频同步时间   
		//				video_original_timestamp_ = audio_original_timestamp_;
		//				video_corrected_timestamp_ = audio_corrected_timestamp_;//
		//			}
		//		}
		//		//return time;
		//	}
		//
		//
		//	int64_t delta = time - video_original_timestamp_; //原始值
		//	//判断是否在范围之内
		//	bool  fine = (delta > -kMaxVideoDeltaTime) && (delta < kMaxVideoDeltaTime);
		//	if (!fine)
		//	{
		//		delta = kMaxVideoDeltaTime;
		//	}
		//
		//	// 更新时间戳
		//	video_original_timestamp_ = time;
		//	video_corrected_timestamp_ += delta;
		//	if (video_corrected_timestamp_ < 0)
		//	{
		//		video_corrected_timestamp_ = 0;
		//	}
		//	return video_corrected_timestamp_;
		//}
		//
		//uint32_t   TimeCorrector::CorrectAudioTimeStampByAudio(const PacketPtr& packet)
		//{
		//	int64_t time = packet->TimeStamp();
		//	if (audio_original_timestamp_ == -1)
		//	{
		//		audio_original_timestamp_ = time;
		//		audio_corrected_timestamp_ = time;
		//		return time;
		//	}
		//
		//
		//	int64_t delta = time - audio_original_timestamp_; //原始值
		//	//判断是否在范围之内
		//	bool  fine = (delta > -kMaxAudioDeltaTime) && (delta < kMaxAudioDeltaTime);
		//	if (!fine)
		//	{
		//		delta = kMaxAudioDeltaTime;
		//	}
		//
		//	// 更新时间戳
		//	audio_original_timestamp_ = time;
		//	audio_corrected_timestamp_ += delta;
		//	if (audio_corrected_timestamp_ < 0)
		//	{
		//		audio_corrected_timestamp_ = 0;
		//	}
		//	return audio_corrected_timestamp_;
		//}

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
	
}