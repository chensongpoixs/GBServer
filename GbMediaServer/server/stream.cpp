

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR


ʵʱ��

1. һ·ʵʱ���������Ʒ������
2. ������������������
3. �������Զ�����




- ������������Ƶ����
	1. ����ʱ����� ����index
	2. ����CodecHeader
	3. ����GOP
	4. ��������Ƶ֡

- �������Ƶ����



1. ��λGOP�� ����CodecHeader
2. �����ȷ���CodecHeader�� �ٷ�������Ƶ����
3. ��֡�����²���CodecHeader
4. ���һ������������Ƶ֡






										   ����ͼ


			  ��������Ƶ֡



			�Ѿ��������Ƶ����ͷ                     �� ===>          |          ��λGOP
																	|
																	|
				  ��												    |
																	|
			����Ƶ֡���̫��?                       ��   ===>          |          ��λGOP

				  ��                                                           ||

																			��������Ƶ����ͷ

																				 ||

																			�������Ƶ����ͷ


			���һ������������Ƶ֡




����ʱ


- ��ʱ��û���յ�ʵʱ���ݣ� ��¼�յ����ݵ�ʱ�� ��ǰʱ���յ����ݵ�ʱ�� ����һ����ʱ�䣬 �����㳬ʱ
- ����ʱ�Ǳ������




��׼����

- �յ��ؼ�֡


��Ӯ����Ҫ���𰸶�������ʲô�������Ҫ��

�����ߣ��ٴ�֮����Ҳ��Ψ�з������ܣ�����������ʱ����ʱ����Ӣ�ۣ�Ӣ�۴�����ʱ�����������˵�����񣬿��㱾�����ٰ��� �����ã��Լ�������ͼ��顣


�ҿ��ܻ������ܶ���ˣ������ǽ���2��Ĺ��£�����д�ɹ��»��ɸ裬����ѧ���ĸ���������������
Ȼ�󻹿�����һ����������һ�������ҵĹ�������ܻᱻ��˧����������ֻᱻ��������ں���������ĳ�������ҹ������ȫ����͸Ȼ��Ҫ������С��ס�������ϵ�ʪ�·���
3Сʱ���������������ʵ��������ҵĹ�������Ը���򸸻���顣�Ҳ����Ѹ��������Ǵ�Խ�����������ǣ���ʼ��Ҫ�ص��Լ������硣
Ȼ���ҵ���Ӱ������ʧ���ҿ������������ˮ������ȴû���κ�ʹ�࣬�Ҳ�֪����ԭ���ҵ��ı������ˣ�������ȫ�����ԭ�򣬾���Ҫ�һ��Լ��ı��ġ�
�����ҿ�ʼ����Ѱ�Ҹ��ָ���ʧȥ�ĵ��ˣ��ұ��һ��שͷ��һ������һ��ˮ��һ����ƣ�ȥ�����Ϊʲô��ʧȥ�Լ��ı��ġ�
�ҷ��֣��ճ����ı��������Ļ��ڣ������ģ����ǵı��ľͻ���ʧ���յ��˸��ֺڰ�֮�����ʴ��
��һ�����ۣ������ʺͱ��ߣ�����ί����ʹ�࣬�ҿ���һֻֻ���ε��֣������ǵı��ĳ��飬�ɱΣ�͵�ߣ���Ҳ�ز������˶���ߡ�
�ҽ����������֡��������Ǻ�����ͬ�ڵļ��� �����Ҳ������£�����ϸ�����Լ�ƽ����һ�� Ѱ�ұ������ֵĺۼ���
�����Լ��Ļ��䣬һ�����ĳ�����������������֣��ҵı��ģ�����д�����ʱ�򣬻������
��������Ȼ����������ҵ�һ�У�д��������ұ��Ļع����÷�ʽ���һ�û�ҵ��������֣��������ţ�˳�������������һ����˳�����ϣ�������������
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
		//	//����ʱ���
		//	int64_t t = time_corrector_.CorrectTimestamp(packet);
		//	packet->SetTimeStamp(t);
		//
		//	std::lock_guard<std::mutex> lk(lock_);
		//	uint32_t index = ++frame_index_;
		//	//LIVE_DEBUG << "add type : " << packet->PacketType();
		//	packet->SetIndex(index);
		//	//�ж��Ƿ�ؼ�֡
		//	if (packet->IsVideo() && CodecUtils::isKeyFrame(packet))
		//	{
		//		SetReady(true);
		//		packet->SetPacketType(kPacketTypeVideo | kFrameTypeKeyFrame);
		//	}
		//	//�ж��Ƿ�������ͷ
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
		//	// ���gop������
		//	int32_t min_idx = frame_index_ - packet_buffer_size_;
		//	if (min_idx > 0)
		//	{
		//		gop_mgr_.ClearExpriedGop(min_idx);
		//	}
		//
		//
		//	if (data_coming_time_ == 0)
		//	{
		//		//��һ�������ݵ�ʱ��
		//		data_coming_time_ = tmms::base::TTime::NowMS();
		//	}
		//	stream_time_ = tmms::base::TTime::NowMS();
		//	auto frame = frame_index_.load();
		//	if (frame < 300 || frame % 5 == 0)
		//	{
		//		session_.ActiveAllPlayers();
		//	}
		//	// ��ǰ֡������
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
			//	//û����
			//	return;
			//}
			//if (user->meta_ || user->audio_header_ || user->video_header_ || !user->out_frames_.empty())
			//{
			//	//�����ݾͼ�������  
			//	return;
			//}
			//
			//std::lock_guard<std::mutex >lk(lock_);
			//if (user->out_index_ >= 0)
			//{
			//	//��Ҫ����Ҫskip frame
			//	int32_t min_idx = frame_index_ - packet_buffer_size_;
			//	int32_t context_lantecy = user->GetAppInfo()->content_latency_;
			//
			//	if (user->out_index_ < min_idx ||
			//		((gop_mgr_.LastestTimeStamp() - user->out_frame_timestamp_) > (2 * context_lantecy)))
			//	{
			//		//��Ҫskip frame 
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
		//��һ֡���ݵ�ʱ��
		int64_t       Stream::DateTime() const
		{
			return data_coming_time_;
		}
		// ����Ϣ����
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
		//�������Ƿ�׼����   �յ��ؼ�֡
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

