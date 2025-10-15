

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		rtmp_header



����Ƶʱ�����������


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
		//	//	// ���������ݰ�����
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
		//	//��������Ƶ��
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
		//	int64_t delta = time - video_original_timestamp_; //ԭʼֵ
		//	//�ж��Ƿ��ڷ�Χ֮��
		//	bool  fine = (delta > -kMaxVideoDeltaTime) && (delta < kMaxVideoDeltaTime);
		//	if (!fine)
		//	{
		//		delta = kMaxVideoDeltaTime;
		//	}
		//
		//	// ����ʱ���
		//	audio_original_timestamp_ = time;
		//	// ͨ����Ƶ������Ƶʱ��� 
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
		//		// ��Ƶ��ת��  ��Ƶ�������� 
		//		if (audio_original_timestamp_ != -1)
		//		{
		//			// diff time
		//			int32_t delta = audio_original_timestamp_ - audio_corrected_timestamp_;
		//			if (delta <= -kMaxVideoDeltaTime || delta >= kMaxVideoDeltaTime)
		//			{
		//				// ��Ƶ����Ƶͬ��ʱ��   
		//				video_original_timestamp_ = audio_original_timestamp_;
		//				video_corrected_timestamp_ = audio_corrected_timestamp_;//
		//			}
		//		}
		//		//return time;
		//	}
		//
		//
		//	int64_t delta = time - video_original_timestamp_; //ԭʼֵ
		//	//�ж��Ƿ��ڷ�Χ֮��
		//	bool  fine = (delta > -kMaxVideoDeltaTime) && (delta < kMaxVideoDeltaTime);
		//	if (!fine)
		//	{
		//		delta = kMaxVideoDeltaTime;
		//	}
		//
		//	// ����ʱ���
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
		//	int64_t delta = time - audio_original_timestamp_; //ԭʼֵ
		//	//�ж��Ƿ��ڷ�Χ֮��
		//	bool  fine = (delta > -kMaxAudioDeltaTime) && (delta < kMaxAudioDeltaTime);
		//	if (!fine)
		//	{
		//		delta = kMaxAudioDeltaTime;
		//	}
		//
		//	// ����ʱ���
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