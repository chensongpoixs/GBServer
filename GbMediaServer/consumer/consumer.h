

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR






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
namespace gb_media_server
{
	 

		enum  ConsumerType
		{ 
			kConsumerTypePlayerPav = 0,
			kConsumerTypePlayerFlv,
			kConsumerTypePlayerHls,
			kConsumerTypePlayerRtmp,
			kConsumerTypePlayerWebRTC,
			kConsumerTypeUnknowed = 255,


		};
		 
		class Stream;
		class Session;
		 
		 
		class Consumer : public std::enable_shared_from_this<Consumer>
		{
		public:
			friend class Session;
			Consumer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
			virtual ~Consumer() = default;
		public:


		 
			const std::string & AppName() const;
			void SetAppName(const std::string & app_name);
			const std::string & StreamName() const;
			void SetStreamName(const std::string & stream);
			const std::string & Param() const;
			void SetParam(const std::string & param);



			 


			virtual ConsumerType GetConsumerType() const;
			void SetConsumerType(ConsumerType t);
			 
			const rtc::SocketAddress &GetRemoteAddress() const
			{
				return remote_address_;
			}
			void  SetRemoteAddress(const rtc::SocketAddress & addr);

			std::shared_ptr<Session> GetSession() const
			{
				return session_;
			}
			std::shared_ptr < Stream> GetStream() const
			{
				return stream_;
			}
			std::shared_ptr < Stream> GetStream()
			{
				return stream_;
			}
			

			//���ܲ�ͬЭ���ϲ㴦�� ʵ��
			virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}


			//���Ŷ˼̳�
			virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame) {}
			virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame) {}
		public:
			//�����ӿ�
			//void Close();
			
		protected:
			std::shared_ptr < Stream> stream_;  
			std::string     app_name_;
			std::string     stream_name_;
			std::string     param_;
			//std::string		user_id_; 
			rtc::SocketAddress   remote_address_;
			int64_t			start_timestamp_{ 0 }; // ��ʼʱ��
			ConsumerType		type_{ ConsumerType::kConsumerTypeUnknowed };
			std::atomic_bool destroyed_{ false };
			std::shared_ptr < Session> session_;
		};
 
}

#endif // 