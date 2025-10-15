

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		PlayerUser

1. �����û���Ҫ������ʵʱ����Ƶ����
2. �����û���Ҫ����ʵʱ��Stream



- ʱ���У��
- CodecHeader���
- ʵʱ����Ƶ�������״̬��Ϣ
- �麯������ӿ�


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

#ifndef _C_GB_MEDIA_SERVER_PLAYER_USER_H_
#define _C_GB_MEDIA_SERVER_PLAYER_USER_H_



#include <cstdint>
#include <memory> 
#include <vector>
#include <string> 
#include  "user.h"
//#include "server/"
namespace gb_media_server
{
	 
	//class User;
		class PlayerUser : public User
		{
		public:
			friend  class Stream;
			explicit PlayerUser(  std::shared_ptr < Connection> &ptr,   std::shared_ptr < Stream> &stream,   std::shared_ptr < Session> &s);
			//ί�й��캯��
			//using User::User;
		public:


			//explicit PlayerUser(const ConnectionPtr & ptr);
		public:
			//PacketPtr Meta() const;
			//PacketPtr AudioHeader() const;
			//PacketPtr VideoHeader() const;
			//void ClearMeta();
			//void ClearAudioHeader();
			//void ClearVideoHeader();



			//StreamPtr Stream();

			virtual bool PostFrames() {
				return false;
			};

			//TimeCorrector & GetTimeCorrector();

		public:
			//PacketPtr   meta_;
			//PacketPtr   audio_header_;
			//PacketPtr   video_header_;


			bool		wait_meta_{ true };
			bool		wait_audio_{ true };
			bool		wait_video_{ true };

			// ��֡�ļ�¼
			int32_t    video_header_index_{ 0 };
			int32_t		audio_header_index_{ 0 };
			int32_t		meta_index_{ 0 };


			//TimeCorrector   time_corrector_;
			bool			wait_timeout_{ false };
			int32_t			out_version_{ -1 };
			int32_t			out_frame_timestamp_{ 0 };
			//std::vector<PacketPtr> out_frames_;
			int32_t			out_index_{ -1 };
		};

 
}

#endif // 