

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
#include "server/stream.h"
#include "server/session.h"
#include "rtc_base/time_utils.h"
#include "user/player_user.h"
namespace gb_media_server
{
	 

		Stream::Stream( Session &s, const std::string & session_name)
			:session_(s), session_name_(session_name)
			//, packet_buffer_(packet_buffer_size_)
			//, hls_muxer_(session_name)
		{
			stream_time_ = rtc::TimeMillis();
			start_timestamp_ = rtc::TimeMillis();
		}
		void Stream::AddVideoFrame(const rtc::CopyOnWriteBuffer& frame)
		{
			// workthread 
			session_.AddVideoFrame(frame);
		}
		void Stream::AddAudioFrame(const rtc::CopyOnWriteBuffer& frame)
		{

		}
		//void Stream::AddPacket(PacketPtr &&packet)
		//{
		//	//矫正时间戳
		//	int64_t t = time_corrector_.CorrectTimestamp(packet);
		//	packet->SetTimeStamp(t);
		//
		//	std::lock_guard<std::mutex> lk(lock_);
		//	uint32_t index = ++frame_index_;
		//	//LIVE_DEBUG << "add type : " << packet->PacketType();
		//	packet->SetIndex(index);
		//	//判断是否关键帧
		//	if (packet->IsVideo() && CodecUtils::isKeyFrame(packet))
		//	{
		//		SetReady(true);
		//		packet->SetPacketType(kPacketTypeVideo | kFrameTypeKeyFrame);
		//	}
		//	//判断是否是序列头
		//
		//	if (CodecUtils::IsCodecHeader(packet))
		//	{
		//		codec_headers_.ParseCodecHeader(packet);
		//
		//		if (packet->IsVideo())
		//		{
		//			has_video_ = true;
		//			++stream_version_;
		//		}
		//		else if (packet->IsAudio())
		//		{
		//			has_audio_ = true;
		//			++stream_version_;
		//		}
		//		else if (packet->IsMeta())
		//		{
		//			has_meta_ = true;
		//			++stream_version_;
		//		}
		//		else
		//		{
		//			// error 
		//		}
		//	}
		//
		//	// 
		//	gop_mgr_.AddFrame(packet);
		//	// mpegts 
		//	//LIVE_DEBUG << "ProcessHlsadd type : " << packet->PacketType();
		//	ProcessHls(packet);
		//	//LIVE_DEBUG << "ProcessHls end type : " << packet->PacketType();
		//	packet_buffer_[(index % packet_buffer_size_)] = std::move(packet);
		//	// 清除gop的数据
		//	int32_t min_idx = frame_index_ - packet_buffer_size_;
		//	if (min_idx > 0)
		//	{
		//		gop_mgr_.ClearExpriedGop(min_idx);
		//	}
		//
		//
		//	if (data_coming_time_ == 0)
		//	{
		//		//第一次来数据的时候
		//		data_coming_time_ = tmms::base::TTime::NowMS();
		//	}
		//	stream_time_ = tmms::base::TTime::NowMS();
		//	auto frame = frame_index_.load();
		//	if (frame < 300 || frame % 5 == 0)
		//	{
		//		session_.ActiveAllPlayers();
		//	}
		//	// 当前帧数大于
		//}
		//
		//void Stream::ProcessHls(PacketPtr & packet)
		//{
		//
		//
		//	if (!session_.GetAppInfo()->hls_support_)
		//	{
		//		return;
		//	}
		//
		//	hls_muxer_.OnPacket(packet);
		//
		//	/*if (CodecUtils::IsCodecHeader(packet))
		//	{
		//		char   *data = packet->Data();
		//		if (packet->IsAudio())
		//		{
		//			AudioCodecID id = (AudioCodecID)((*data & 0XF0) >> 4);
		//			encoder_.SetStreamType(&writer_, kVideoCodecIDReserved, id);
		//		}
		//		if (packet->IsVideo())
		//		{
		//			VideoCodecID id = (VideoCodecID)((*data & 0X0F)  );
		//			encoder_.SetStreamType(&writer_,id,  kAudioCodecIDReserved   );
		//		}
		//	}
		//	encoder_.Encode(&writer_, packet, packet->TimeStamp());*/
		//}

		void Stream::GetFrames(const std::shared_ptr<PlayerUser>& user)
		{
			//if (!HasMedia())
			//{
			//	//没有流
			//	return;
			//}
			//if (user->meta_ || user->audio_header_ || user->video_header_ || !user->out_frames_.empty())
			//{
			//	//有数据就继续发送  
			//	return;
			//}
			//
			//std::lock_guard<std::mutex >lk(lock_);
			//if (user->out_index_ >= 0)
			//{
			//	//需要不需要skip frame
			//	int32_t min_idx = frame_index_ - packet_buffer_size_;
			//	int32_t context_lantecy = user->GetAppInfo()->content_latency_;
			//
			//	if (user->out_index_ < min_idx ||
			//		((gop_mgr_.LastestTimeStamp() - user->out_frame_timestamp_) > (2 * context_lantecy)))
			//	{
			//		//需要skip frame 
			//
			//		LIVE_INFO << "need skip out index : " << user->out_index_
			//			<< ", min idx: " << min_idx
			//			<< ", out timestamp: " << user->out_frame_timestamp_
			//			<< ", latest timestamp: " << gop_mgr_.LastestTimeStamp()
			//			<< ", context_lantecy: " << context_lantecy;
			//		SkipFrame(user);
			//	}
			//}
			//else
			//{
			//	if (!LocateGop(user))
			//	{
			//		return;
			//	}
			//}
			GetNextFrame(user);
		}
		bool Stream::LocateGop(const std::shared_ptr<PlayerUser>& user)
		{
			//int32_t context_lantecy = user->GetAppInfo()->content_latency_;
			//
			//int32_t  lantency = 0;
			//int32_t idx = gop_mgr_.GetGopByLatency(context_lantecy, lantency);
			//
			//if (idx != -1)
			//{
			//	user->out_index_ = idx - 1;
			//}
			//else
			//{
			//	int64_t  elapsed = user->ElapsedTime();
			//	if (elapsed >= 10000 && !user->wait_timeout_)
			//	{
			//		LIVE_DEBUG << "wait Gop keyframe timeout. host: " << user->user_id_;
			//		user->wait_timeout_ = true;
			//
			//	}
			//	return false;
			//}
			//
			//
			//user->wait_meta_ = (user->wait_meta_ && has_meta_);
			//if (user->wait_meta_)
			//{
			//	//
			//	PacketPtr meta = codec_headers_.Meta(idx);
			//	if (meta)
			//	{
			//		user->wait_meta_ = false;
			//		user->meta_ = meta;
			//		user->meta_index_ = meta->Index();
			//	}
			//}
			//user->wait_audio_ = (user->wait_audio_ && has_audio_);
			//if (user->wait_audio_)
			//{
			//	//
			//	PacketPtr audio = codec_headers_.AudioHeader(idx);
			//	if (audio)
			//	{
			//		user->wait_audio_ = false;
			//		user->audio_header_ = audio;
			//		user->audio_header_index_ = audio->Index();
			//	}
			//}
			//user->wait_video_ = (user->wait_video_ && has_video_);
			//if (user->wait_video_)
			//{
			//	//
			//	PacketPtr video = codec_headers_.VideoHeader(idx);
			//	if (video)
			//	{
			//		user->wait_video_ = false;
			//		user->video_header_ = video;
			//		user->video_header_index_ = video->Index();
			//	}
			//}
			//if (user->wait_audio_ || user->wait_video_ || user->wait_meta_ || idx == -1)
			//{
			//	int64_t  elapsed = user->ElapsedTime();
			//	if (elapsed >= 10000 && !user->wait_timeout_)
			//	{
			//		LIVE_DEBUG << "wait Gop keyframe timeout elapsed : " << elapsed
			//			<< "ms , frame_index: " << frame_index_.load()
			//			<< ", gop size: " << gop_mgr_.GopSize()
			//			<< ", . host: " << user->user_id_;
			//		user->wait_timeout_ = true;
			//
			//	}
			//	return false;
			//}
			//
			//user->wait_audio_ = true;
			//user->wait_video_ = true;
			//user->wait_meta_ = true;
			//user->out_version_ = stream_version_;
			//int64_t elapsed = user->ElapsedTime();
			//
			//LIVE_DEBUG << " locate GOP success. eapsed : " << elapsed
			//	<< ", ms gop idx: " << idx
			//	<< ", frame index: " << frame_index_.load()
			//	<< ", lantency: " << lantency
			//	<< ", user: " << user->user_id_;
			return true;
		}
		void Stream::SkipFrame(const std::shared_ptr<PlayerUser>& user)
		{
			//int32_t context_lantecy = user->GetAppInfo()->content_latency_;
			//
			//int32_t  lantency = 0;
			//int32_t idx = gop_mgr_.GetGopByLatency(context_lantecy, lantency);
			//
			//if (idx == -1 || idx <= user->out_index_)
			//{
			//	return;
			//}
			//
			//
			//PacketPtr meta = codec_headers_.Meta(idx);
			//if (meta)
			//{
			//	if (meta->Index() > user->meta_index_)
			//	{
			//		user->meta_ = meta;
			//		user->meta_index_ = meta->Index();
			//	}
			//}
			//PacketPtr audio = codec_headers_.AudioHeader(idx);
			//if (audio)
			//{
			//	if (audio->Index() > user->audio_header_index_)
			//	{
			//		user->audio_header_ = audio;
			//		user->audio_header_index_ = audio->Index();
			//	}
			//}
			//PacketPtr video = codec_headers_.VideoHeader(idx);
			//if (video)
			//{
			//	if (video->Index() > user->video_header_index_)
			//	{
			//		user->video_header_ = video;
			//		user->video_header_index_ = video->Index();
			//	}
			//}
			//
			//LIVE_DEBUG << "skip frame " << user->out_index_ << " -> " << idx
			//	<< ", lantency: " << lantency
			//	<< ", frame_index: " << frame_index_.load()
			//	<< ", host : " << user->user_id_;
			//user->out_index_ = idx - 1;
		}
		void Stream::GetNextFrame(const std::shared_ptr<PlayerUser>& user)
		{

			//int64_t idx = user->out_index_ + 1;
			//int64_t max_idx = frame_index_.load();
			//for (int32_t i = 0; i < 10; ++i)
			//{
			//	if (idx > max_idx)
			//	{
			//		break;
			//	}
			//
			//	PacketPtr & pkt = packet_buffer_[idx %packet_buffer_size_];
			//	if (pkt)
			//	{
			//		user->out_frames_.emplace_back(pkt);
			//		user->out_index_ = pkt->Index();
			//		user->out_frame_timestamp_ = pkt->TimeStamp();
			//		idx = pkt->Index() + 1;
			//	}
			//	else
			//	{
			//		break;
			//	}
			//}

		}


		//bool   Stream::HasAudio() const
		//{
		//	return has_audio_;
		//}
		//bool   Stream::HasVideo() const
		//{
		//	return has_video_;
		//}
		int64_t Stream::ReadyTime() const
		{
			return  ready_time_;
		}
		int64_t Stream::SinceStart() const
		{
			return rtc::TimeMillis() - start_timestamp_;
		}
		bool	Stream::Timeout()
		{
			int64_t  delta = rtc::TimeMillis() - stream_time_;
			if (delta > 20 * 1000)
			{
				return true;
			}
			return false;
		}
		//第一帧数据的时间
		int64_t       Stream::DateTime() const
		{
			return data_coming_time_;
		}
		// 流信息函数
		const std::string & Stream::SessionName() const
		{
			return session_name_;
		}
		int32_t Stream::StreamVersion() const
		{
			return stream_version_;
		}
		//bool Stream::HasMedia() const
		//{
		//	return  has_audio_ || has_video_ || has_meta_;
		//}
		//设置流是否准备好   收到关键帧
		void Stream::SetReady(bool ready)
		{
			ready_ = ready;
			ready_time_ = rtc::TimeMillis();
		}
		bool Stream::Ready() const
		{
			return ready_;
		}
	 
}

