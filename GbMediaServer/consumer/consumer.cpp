

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
				{ // sps�б��ڷ���sps��pps����Ϣ
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