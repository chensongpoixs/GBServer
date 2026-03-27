

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
#include "gb_media_server_log.h"


#define   SHOW_NAL_TYPE  (0)


namespace gb_media_server
{ 
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 消费者构造函数（Consumer Constructor）
	*  
	*  该构造函数用于初始化消费者对象，设置视频缓冲区、创建媒体复用器（Muxer），
	*  并连接音视频编码信号槽，用于处理编码后的音视频数据。
	*  
	*  初始化流程：
	*  1. 调用基类 ShareResource 的构造函数，传入 Stream 和 Session 对象
	*  2. 初始化 SPS/PPS 缓冲区为空
	*  3. 设置 send_sps_pps_ 标志为 false，表示尚未发送 SPS/PPS
	*  4. 分配 8MB 的视频帧缓冲区（1024 * 1024 * 8 字节）
	*  5. 创建 Muxer 对象，用于媒体数据的复用和转换
	*  6. 连接 Muxer 的音频编码信号到 SendAudioEncode 方法
	*  7. 连接 Muxer 的视频编码信号到 SendVideoEncode 方法
	*  
	*  @param stream 流对象的共享指针，用于管理媒体流的生命周期
	*  @param s 会话对象的共享指针，用于管理会话的生命周期
	*  @note 视频缓冲区大小为 8MB，足以容纳大多数视频帧
	*  @note Muxer 用于将原始音视频数据转换为特定格式（如 FLV、HLS 等）
	*  @note 信号槽机制用于异步处理编码后的音视频数据
	*/
	Consumer::Consumer(  const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s)
			:ShareResource(stream, s)
			, sps_()
			, pps_()
			, send_sps_pps_(false)

		, video_buffer_frame_(1024 * 1024 * 8) 
		, muxer_(nullptr)
		{
			GBMEDIASERVER_LOG_T_F(LS_INFO);
			muxer_ = new libmedia_transfer_protocol::Muxer();
			muxer_->SignalAudioEncoderInfoFrame.connect(this, &Consumer::SendAudioEncode);
			muxer_->SignalVideoEncodedImage.connect(this, &Consumer::SendVideoEncode);
		}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 消费者析构函数（Consumer Destructor）
	*  
	*  该析构函数用于清理消费者对象，断开所有信号槽连接，释放 Muxer 对象占用的资源。
	*  
	*  清理流程：
	*  1. 记录析构日志
	*  2. 检查 Muxer 对象是否存在
	*  3. 断开音频编码信号槽的所有连接
	*  4. 断开视频编码信号槽的所有连接
	*  5. 删除 Muxer 对象，释放内存
	*  6. 将 Muxer 指针设置为 nullptr，避免悬空指针
	*  
	*  @note 必须先断开信号槽连接，再删除 Muxer 对象，避免回调到已销毁的对象
	*  @note 视频缓冲区（video_buffer_frame_）会自动释放，无需手动清理
	*  @note SPS/PPS 缓冲区会自动释放，无需手动清理
	*/
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
 
	}

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 添加视频帧（Add Video Frame）
	*  
	*  该方法用于处理 H264 编码的视频帧，解析 NALU 单元，提取 SPS/PPS 参数，
	*  并在 IDR 帧前附加 SPS/PPS，确保解码器能够正确解码视频。
	*  
	*  处理流程：
	*  1. 使用 H264::FindNaluIndices 解析视频帧，识别所有 NALU 单元
	*  2. 清空视频缓冲区，准备存储处理后的视频帧
	*  3. 遍历所有 NALU 单元，根据类型进行不同处理：
	*     - SPS（Sequence Parameter Set）：提取并缓存到 sps_ 成员变量
	*     - PPS（Picture Parameter Set）：提取并缓存到 pps_ 成员变量
	*     - IDR 帧：在帧前附加 SPS 和 PPS，确保解码器能够快速恢复
	*     - Slice 帧：直接添加到视频缓冲区
	*     - 其他 NALU 类型（AUD、SEI、Filler 等）：直接添加到视频缓冲区
	*  4. 设置 send_sps_pps_ 标志为 true，表示已发送 SPS/PPS
	*  5. 创建新的 EncodedImage 对象，包含处理后的视频帧
	*  6. 调用 OnVideoFrame 方法，将视频帧传递给具体的消费者实现
	*  
	*  NALU 类型说明：
	*  - SPS（7）：序列参数集，包含视频分辨率、帧率、色彩空间等信息
	*  - PPS（8）：图像参数集，包含熵编码模式、量化参数等信息
	*  - IDR（5）：即时解码刷新帧，关键帧，可以独立解码
	*  - Slice（1）：非关键帧，依赖前面的帧进行解码
	*  - AUD（9）：访问单元分隔符，用于标识帧边界
	*  - SEI（6）：补充增强信息，包含额外的元数据
	*  
	*  @param frame 编码后的视频帧，包含 H264 编码数据和元信息
	*  @note 该方法会解析 H264 NALU 单元，并在 IDR 帧前自动附加 SPS 和 PPS
	*  @note 如果视频缓冲区为空，则不会调用 OnVideoFrame 方法
	*  @note 视频缓冲区大小为 8MB，足以容纳大多数视频帧
	*/
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
				video_buffer_frame_.AppendData(rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4));
				//video_buffer_frame_.SetSize( frame.size());
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
				video_buffer_frame_.AppendData(rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4));
				//rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4)video_buffer_frame_.SetSize(  frame.size());
				break;
			}
			case webrtc::H264::NaluType::kStapA:
			case webrtc::H264::NaluType::kFuA:
			{
#if SHOW_NAL_TYPE
				 GBMEDIASERVER_LOG_T_F(LS_INFO) << "========" << nal_index << "==========>kFuA---kStapA  , size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
				video_buffer_frame_.AppendData(rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4));
				//video_buffer_frame_.SetSize(  frame.size());
				break;
			}
			default: {
#if SHOW_NAL_TYPE
				 GBMEDIASERVER_LOG_T_F(LS_INFO) << "=====" << nal_index << "=============>default  packet  , size:" << nalus[nal_index].payload_size;
#endif //#if SHOW_NAL_TYPE
				video_buffer_frame_.AppendData(rtc::CopyOnWriteBuffer(frame.data() + nalus[nal_index].start_offset, nalus[nal_index].payload_size + 4));
				//video_buffer_frame_.SetSize(  frame.size());
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

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 添加音频帧（Add Audio Frame）
	*  
	*  该方法用于处理音频帧，直接将音频编码数据和 PTS 时间戳传递给 OnAudioFrame 方法。
	*  音频帧的处理相对简单，不需要像视频帧那样解析 NALU 单元或附加参数集。
	*  
	*  处理流程：
	*  1. 接收音频编码数据缓冲区和 PTS 时间戳
	*  2. 直接调用 OnAudioFrame 方法，将音频帧传递给具体的消费者实现
	*  3. 由具体的消费者实现类（如 RtcConsumer、FlvConsumer 等）处理音频帧的封装和发送
	*  
	*  音频数据说明：
	*  - 音频帧通常包含一个或多个音频采样周期的编码数据
	*  - 音频编码格式可能为 AAC、OPUS 等，由具体的消费者实现决定
	*  - PTS 用于同步音频和视频的播放时间
	*  
	*  @param frame 音频编码数据缓冲区，包含压缩后的音频数据
	*  @param pts 呈现时间戳（Presentation Time Stamp），单位通常为毫秒或采样周期
	*  @note 音频帧处理不需要像视频帧那样附加 SPS/PPS 等参数
	*  @note 该方法会调用 OnAudioFrame 方法将音频帧传递给具体的消费者实现
	*/
	void Consumer::AddAudioFrame(const rtc::CopyOnWriteBuffer & frame, int64_t pts)
	{
		OnAudioFrame(frame, pts);
	}
 
	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 发送视频编码帧（Send Video Encode）
	*  
	*  该方法是 Muxer 的信号槽回调，当 Muxer 完成视频编码后触发。
	*  该方法接收编码后的视频帧，并将其传递给 OnVideoFrame 方法进行发送。
	*  
	*  处理流程：
	*  1. 接收 Muxer 编码后的视频帧（EncodedImage 智能指针）
	*  2. 解引用智能指针，获取 EncodedImage 对象
	*  3. 调用 OnVideoFrame 方法，将视频帧传递给具体的消费者实现
	*  4. 由具体的消费者实现类（如 RtcConsumer、FlvConsumer 等）处理视频帧的封装和发送
	*  
	*  使用场景：
	*  - 当使用 Muxer 进行视频转码或格式转换时，该方法会被自动调用
	*  - 例如：将 H265 转码为 H264，或将原始 YUV 编码为 H264
	*  
	*  @param encoded_image 编码后的视频帧智能指针，包含 H264 编码数据和元信息
	*  @note 该方法由 Muxer 的信号槽机制自动调用，不需要手动调用
	*  @note 智能指针会自动管理内存，无需手动释放
	*  @note 注释掉的代码用于调试，可以将视频帧保存到缓冲区
	*/
	void Consumer::SendVideoEncode(std::shared_ptr<libmedia_codec::EncodedImage> encoded_image)
	{
		// rtc::CopyOnWriteBuffer  buffer;
		 //buffer.AppendData(*encoded_image.get());
		OnVideoFrame(*encoded_image.get());
		//buffer.Clear();



		return;

	}


	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief 发送音频编码帧（Send Audio Encode）
	*  
	*  该方法是 Muxer 的信号槽回调，当 Muxer 完成音频编码后触发。
	*  该方法接收编码后的音频帧，并将其封装为 RTP 包通过 SRTP 发送。
	*  
	*  处理流程（注释掉的代码）：
	*  1. 在工作线程中异步处理音频帧
	*  2. 检查 DTLS 握手是否完成，未完成则直接返回
	*  3. 计算 RTP 时间戳（音频时间戳 * 90）
	*  4. 检查 SRTP 发送会话是否存在，不存在则记录警告并返回
	*  5. 创建 RTP 包，设置 Payload Type、时间戳、SSRC 等参数
	*  6. 分配 RTP 包的 Payload 空间，并复制音频编码数据
	*  7. 设置 RTP 包的序列号，并标记为音频包
	*  8. 使用 SRTP 会话加密 RTP 包
	*  9. 通过 RTC 服务器发送加密后的 RTP 包到远程地址
	*  
	*  @param audio_frame 编码后的音频帧智能指针，包含音频编码数据和元信息
	*  @note 该方法由 Muxer 的信号槽机制自动调用，不需要手动调用
	*  @note 当前实现被注释掉（#if 0），可能是因为音频发送逻辑已移至其他地方
	*  @note RTP 时间戳计算公式：音频时间戳 * 90（90kHz 时钟频率）
	*  @note SRTP 用于加密 RTP 包，确保音频数据的安全传输
	*/
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

		 
	 
}