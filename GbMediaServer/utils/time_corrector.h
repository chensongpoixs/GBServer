

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

#ifndef _C_TIME_CORRECTOR_H_
#define _C_TIME_CORRECTOR_H_



#include <cstdint>
#include <memory> 
#include <vector>
#include <string>
namespace gb_media_server
{ 
	size_t split(std::string source,
		char delimiter,
		std::vector<std::string>* fields);
		//class  TimeCorrector
		//{
		//public:
		//	//private://kMaxVideoDeltatTime
		//	const int32_t kMaxVideoDeltaTime = 100;
		//	const int32_t kDefaultVideoDeltaTime = 40;
		//
		//	const int32_t kMaxAudioDeltaTime = 100;
		//	const int32_t kDefaultAudioDeltaTime = 20;
		//public:
		//	TimeCorrector() = default;
		//	~TimeCorrector() = default;
		//
		//
		//	uint32_t   CorrectTimestamp(const PacketPtr &packet);
		//	uint32_t   CorrectAudioTimeStampByVideo(const PacketPtr& packet);
		//	uint32_t   CorrectVideoTimeStampByVideo(const PacketPtr& packet);
		//	uint32_t   CorrectAudioTimeStampByAudio(const PacketPtr& packet);
		//private:
		//	// audio and video 
		//	// û������ǰ��ʱ���
		//	int64_t    video_original_timestamp_{ -1 };
		//	//�޸ĺ�ʱ���
		//	int64_t    video_corrected_timestamp_{ 0 };
		//
		//
		//	int64_t    audio_original_timestamp_{ -1 };
		//	int64_t    audio_corrected_timestamp_{ 0 };
		//	int32_t    audio_numbers_between_video_{ 0 };
		//};
	 
}


#endif //// 

