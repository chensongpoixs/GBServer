

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
#include "share/share_resource.h"

#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 
#include "libmedia_transfer_protocol/muxer/muxer.h"
namespace gb_media_server
{
	 

		 
		 
		
		 
		 
		class Consumer : public ShareResource, public sigslot::has_slots<>
		{
		public:
			Consumer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
			virtual ~Consumer() ;
		public:


		 
		public:
			void SetCapture(bool value);


			void StartCapture();
			void StopCapture();

			// ���زɼ������ݽ��б������з��͵Ľӿ�
			void   SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> f);
			void   SendAudioEncode(std::shared_ptr<libmedia_codec::AudioEncoder::EncodedInfoLeaf> f);
		public:

			//���ܲ�ͬЭ���ϲ㴦�� ʵ��
			//virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}

			/**
			   video :��Ҫ�ȷ���sps_ ��pps_ ����������ð�
			   audio: ������ֱ��ת��
			*/
			void     AddVideoFrame(const libmedia_codec::EncodedImage &frame);
			void     AddAudioFrame(const rtc::CopyOnWriteBuffer& frame);
			//���Ŷ˼̳�
			virtual void OnVideoFrame(const libmedia_codec::EncodedImage &frame) {}
			virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame) {}
		public:
			//�����ӿ�
			//void Close();
			
		protected:
			rtc::CopyOnWriteBuffer                     sps_;
			rtc::CopyOnWriteBuffer						pps_;

			bool                                         send_sps_pps_;
			rtc::Buffer                                video_buffer_frame_;

#if TEST_RTC_PLAY

			bool										capture_type_;//�ɼ����滭�沥��
			std::unique_ptr< rtc::Thread>        video_encoder_thread_;
			std::unique_ptr< libmedia_codec::X264Encoder>                          x264_encoder_;
			rtc::scoped_refptr<libcross_platform_collection_render::CapturerTrackSource>     capturer_track_source_;
			
#endif //
			libmedia_transfer_protocol::Muxer    *      muxer_;
		};
 
}

#endif // 