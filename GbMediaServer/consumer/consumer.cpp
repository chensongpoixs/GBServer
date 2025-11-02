

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR






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
 
#include "rtc_base/time_utils.h" 
#include "server/session.h" 
#include "consumer/consumer.h"
#include "server/stream.h"
#include "common_video/h264/h264_common.h"

#define   SHOW_NAL_TYPE  (0)


namespace gb_media_server
{ 
	Consumer::Consumer(  const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s)
			:ShareResource(stream, s)
			, sps_()
			, pps_()
			, send_sps_pps_(false)

		, video_buffer_frame_(1024 * 1024 * 8)
		, capture_type_(false)
		, muxer_(nullptr)
		{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		muxer_ = new libmedia_transfer_protocol::Muxer();
		muxer_->SignalAudioEncoderInfoFrame.connect(this, &Consumer::SendAudioEncode);
		muxer_->SignalVideoEncodedImage.connect(this, &Consumer::SendVideoEncode);
		}

	Consumer::~Consumer()
	{
		GBMEDIASERVER_LOG_T_F(LS_INFO);
		if (muxer_)
		{
			muxer_->SignalAudioEncoderInfoFrame.disconnect_all();
			muxer_->SignalVideoEncodedImage.disconnect_all();
			delete muxer_;
			muxer_ = nullptr;
		}
		if (x264_encoder_)
		{
			x264_encoder_->SignalVideoEncodedImage.disconnect_all();
		}
#if TEST_RTC_PLAY
		{
			if (capture_type_)
				if (video_encoder_thread_)
				{
					video_encoder_thread_->Stop();
				}
			if (x264_encoder_)
			{
				//	x264_encoder_->SetSendFrame(nullptr);
				x264_encoder_->Stop();
			}
			if (capturer_track_source_)
			{
				//	capturer_track_source_->set_catprue_callback(nullptr, nullptr);
				capturer_track_source_->Stop();
			}
		}

#endif // 	
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
#if SHOW_NAL_TYPE
				  GBMEDIASERVER_LOG_T_F(LS_INFO) << "======"<<nal_index <<"============>SPS, size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
					sps_ = rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4);
				 

				break;
			}
			case webrtc::H264::NaluType::kPps: {
#if SHOW_NAL_TYPE
				GBMEDIASERVER_LOG_T_F(LS_INFO) << "=========" << nal_index << "=========>PPS, size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
					pps_ = rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4);;

				 
				break;
			}
			case webrtc::H264::NaluType::kIdr:
			{
#if SHOW_NAL_TYPE
				GBMEDIASERVER_LOG_T_F(LS_INFO) << "=======" << nal_index << "===========>IDR, size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
				//if (!send_sps_pps_)
				{ // sps有变在发送sps和pps的信息
					video_buffer_frame_.AppendData(sps_);
					video_buffer_frame_.AppendData(pps_);
				}
				video_buffer_frame_.AppendData(rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4));
				video_buffer_frame_.SetSize(  sps_.size() + pps_.size() + nalus[nal_index].payload_size + 4);
				 
				break;
			}
			case webrtc::H264::NaluType::kSlice: {
#if SHOW_NAL_TYPE
				GBMEDIASERVER_LOG_T_F(LS_INFO) << "======" << nal_index << "============>kSlice, size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
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
#if SHOW_NAL_TYPE
				 GBMEDIASERVER_LOG_T_F(LS_INFO) << "=======" << nal_index << "===========>kAud, size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
				video_buffer_frame_.AppendData(frame);
				video_buffer_frame_.SetSize(  frame.size());
				break;
			}
			case webrtc::H264::NaluType::kStapA:
			case webrtc::H264::NaluType::kFuA:
			{
#if SHOW_NAL_TYPE
				 GBMEDIASERVER_LOG_T_F(LS_INFO) << "========" << nal_index << "==========>kFuA---kStapA  , size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
				video_buffer_frame_.AppendData(frame);
				video_buffer_frame_.SetSize(  frame.size());
				break;
			}
			default: {
#if SHOW_NAL_TYPE
				 GBMEDIASERVER_LOG_T_F(LS_INFO) << "=====" << nal_index << "=============>default  packet  , size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
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

	void Consumer::AddAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	{
		OnAudioFrame(frame, pts);
	}
		 
#if TEST_RTC_PLAY

	void Consumer::SetCapture(bool value)
	{
		capture_type_ = value;
	}
	void Consumer::StartCapture()
	{
#if TEST_RTC_PLAY
		if (capture_type_)
		{
			x264_encoder_ = std::make_unique<libmedia_codec::X264Encoder>();
			x264_encoder_->SignalVideoEncodedImage.connect(this, &Consumer::SendVideoEncode);
			x264_encoder_->Start();
			video_encoder_thread_ = rtc::Thread::Create();
			video_encoder_thread_->SetName("video_encoder_thread", NULL);
			video_encoder_thread_->Start();

			capturer_track_source_ = libcross_platform_collection_render::CapturerTrackSource::Create(false);
			capturer_track_source_->set_catprue_callback(x264_encoder_.get(), video_encoder_thread_.get());
			capturer_track_source_->StartCapture();
		}

#endif // 1
	}
	void Consumer::StopCapture()
	{
#if TEST_RTC_PLAY
		if (capture_type_)
		{
			if (video_encoder_thread_)
			{
				video_encoder_thread_->Stop();
			}
			if (x264_encoder_)
			{
				//	x264_encoder_->SetSendFrame(nullptr);
				x264_encoder_->Stop();
			}
			if (capturer_track_source_)
			{
				//	capturer_track_source_->set_catprue_callback(nullptr, nullptr);
				capturer_track_source_->Stop();
			}
		}
		// GetSession()->CloseUser()
#endif //
	}
	void Consumer::SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> encoded_image)
	{
		// rtc::CopyOnWriteBuffer  buffer;
		 //buffer.AppendData(*encoded_image.get());
		OnVideoFrame(*encoded_image.get());
		//buffer.Clear();



		return;

	}


	void Consumer::SendAudioEncode(std::shared_ptr<libmedia_codec::AudioEncoder::EncodedInfoLeaf> audio_frame)
	{
#if 0
		GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [=]() {
			if (!dtls_done_)
			{
				return;
			}


			uint32_t rtp_timestamp = audio_frame->encoded_timestamp * 90;

			if (!srtp_send_session_)
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "ignoring RTP packet due to non sending SRTP session";
				return;
			}

			auto  single_packet =
				std::make_unique<libmedia_transfer_protocol::RtpPacketToSend>(&rtp_header_extension_map_);
			single_packet->SetPayloadType(sdp_.GetAudioPayloadType());
			single_packet->SetTimestamp(rtp_timestamp);
			single_packet->SetSsrc(sdp_.AudioSsrc());
			single_packet->ReserveExtension<libmedia_transfer_protocol::TransportSequenceNumber>();
			uint8_t* payload = single_packet->AllocatePayload(audio_frame->encoded_bytes);
			if (!payload)  // Too large payload buffer.
			{
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << "alloc audio payload size:" << audio_frame->encoded_bytes << " failed !!!";
				return;
			}
			memcpy(payload, audio_frame->audio_encode_data.data(), audio_frame->encoded_bytes);
			//  //int16_t   packet_id = transprot_seq_++;
			single_packet->SetSequenceNumber(audio_seq_++);
			single_packet->set_packet_type(libmedia_transfer_protocol::RtpPacketMediaType::kAudio);



			const uint8_t *data = single_packet->data();
			size_t   len = single_packet->size();
			if (!srtp_send_session_->EncryptRtp(&data, &len))
			{
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << "srtp session ecryptrtp failed !!! ";
				return;
			}


			GbMediaService::GetInstance().GetRtcServer()->SendRtpPacketTo(rtc::CopyOnWriteBuffer(data, len), remote_address_, rtc::PacketOptions());
		});
#endif // 
	}
#endif //

		 
	 
}