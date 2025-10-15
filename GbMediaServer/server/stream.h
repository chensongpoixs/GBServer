

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

#ifndef _C_GB_MEDIA_SERVER_STREAM_H_
#define _C_GB_MEDIA_SERVER_STREAM_H_



#include <cstdint>
#include <memory>
#include <string>
#include <atomic>
#include <mutex>


namespace gb_media_server
{
	 
		  
		class PlayerUser;
		//using PlayerUserPtr =  std::shared_ptr<PlayerUser>;
		class Session;
		class Stream : public std::enable_shared_from_this<Stream>
		{
		public:
			explicit Stream( Session &s, const std::string & session_name);



		public:
		//	void AddPacket(PacketPtr &&packet);

			//void ProcessHls(PacketPtr & packet);
		public:





			void GetFrames(const std::shared_ptr<PlayerUser>& user);





			// ����ͷ
			bool LocateGop(const std::shared_ptr<PlayerUser>& user);
			void SkipFrame(const std::shared_ptr<PlayerUser>& user);

			// �������Ƶ֡
			void GetNextFrame(const std::shared_ptr<PlayerUser>& user);
		public:

			int64_t   ReadyTime() const;
			int64_t   SinceStart() const;
			bool		Timeout();
			//��һ֡���ݵ�ʱ��
			int64_t       DateTime() const;



			// ����Ϣ����
			const std::string & SessionName() const;
			int32_t StreamVersion() const;


			//bool   HasAudio() const;
			//bool   HasVideo() const;
			//// �Ƿ��յ�ý����Ϣ
			//bool HasMedia() const;



			//std::string  GetPlayList()
			//{
			//	return hls_muxer_.PlayList();
			//}
			//FragmentPtr  GetFragement(const std::string &name)
			//{
			//	return hls_muxer_.GetFragment(name);
			//}

			//�������Ƿ�׼����   �յ��ؼ�֡
			void SetReady(bool ready);
			bool Ready() const;


		private:



			int64_t   data_coming_time_{ 0 };
			int64_t   start_timestamp_{ 0 };
			int64_t ready_time_{ 0 };
			std::atomic<int64_t>  stream_time_{ 0 };
			 Session  &session_;
			std::string    session_name_;
			std::atomic<int64_t>   frame_index_{ -1 };
			//Ĭ�ϻ������ݰ���С
			uint32_t   packet_buffer_size_{ 1000 };
			//std::vector<PacketPtr> packet_buffer_;

			bool  has_audio_{ false };
			bool  has_video_{ false };
			bool  has_meta_{ false };

			bool ready_{ false };
			//���İ汾��
			std::atomic< int32_t>   stream_version_{ -1 };


			//GopMgr gop_mgr_;
		//	CodecHeader  codec_headers_;
		//	TimeCorrector time_corrector_;
			std::mutex   lock_;


			/*TestStreamWriter   writer_;
			TsEncoder		   encoder_;*/

			//HLSMuxer 	hls_muxer_;

		};
	 
}


#endif 