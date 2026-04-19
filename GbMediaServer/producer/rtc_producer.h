/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18

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

 ******************************************************************************/


#ifndef _C_GB_MEDIA_SERVER_RTC_PRODUCER_H_
#define _C_GB_MEDIA_SERVER_RTC_PRODUCER_H_

#include <algorithm>

#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include <memory>
 
#include "libmedia_transfer_protocol/librtc/rtc_sdp.h"
#include "libmedia_transfer_protocol/librtc/dtls_certs.h" 
#include "libmedia_transfer_protocol/librtc/dtls.h"
#include "rtc_base/socket_address.h"
#include "libmedia_transfer_protocol/librtc/srtp_session.h"
#if TEST_RTC_PLAY
#include "libmedia_codec/x264_encoder.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#endif // 


#include "libmedia_transfer_protocol/rtp_rtcp/rtp_header_extension_map.h"
#include "libmedia_transfer_protocol/libmpeg/mpeg_decoder.h"
#include "producer/producer.h"
#include "share/rtc_interface.h"
#include "libmedia_transfer_protocol/librtcp/rtcp_context_recv.h"

#include "libmedia_codec/video_codecs/nal_parse_factory.h"
#include "producer/producer_statistics.h"
#include "share/nack_generator.h"

namespace gb_media_server {

	/**
	*  @author chensong
	*  @date 2025-10-18
	*  @brief RTC生产者类（RTC Producer）
	*  
	*  RtcProducer是GBMediaServer流媒体服务器中用于WebRTC协议推流的生产者类。
	*  它继承自RtcInterface、Producer和sigslot::has_slots<>，负责接收客户端通过WebRTC协议
	*  推送的媒体流，并将其转换为服务器内部的媒体流格式。
	*  
	*  WebRTC协议说明：
	*  - WebRTC（Web Real-Time Communication）是一种实时通信技术，支持低延迟的
	*    音视频传输和数据通道
	*  - WebRTC使用SDP（Session Description Protocol）进行信令交换
	*  - WebRTC使用DTLS（Datagram Transport Layer Security）进行密钥交换
	*  - WebRTC使用SRTP（Secure Real-time Transport Protocol）进行媒体加密传输
	*  - WebRTC使用SCTP（Stream Control Transmission Protocol）进行数据通道传输
	*  
	*  RtcProducer功能：
	*  1. 接收来自客户端的RTP/RTCP包并解密
	*  2. 解析RTP包，提取音视频数据并重构为完整的帧
	*  3. 处理RTCP控制包（接收报告、发送报告、反馈等）
	*  4. 支持DTLS握手和SRTP解密
	*  5. 支持SCTP数据通道
	*  6. 将接收到的媒体帧推送到Stream中
	*  
	*  工作流程：
	*  1. 客户端通过HTTP API发送SDP Offer
	*  2. RtcProducer处理SDP Offer并生成SDP Answer
	*  3. 开始DTLS握手，交换证书和密钥
	*  4. DTLS握手成功后，建立SRTP会话
	*  5. 建立SCTP关联，支持数据通道
	*  6. 开始接收和解密RTP媒体包和RTCP控制包
	*  7. 解析RTP包并重构媒体帧，推送到Stream
	*  
	*  RTP/RTCP说明：
	*  - RTP（Real-time Transport Protocol）用于传输音视频数据
	*  - RTCP（RTP Control Protocol）用于传输控制信息（丢包统计、带宽估计等）
	*  - RTP包包含序列号、时间戳、SSRC等头部信息
	*  - RTCP包用于反馈接收质量、请求关键帧等
	*  
	*  @note RtcProducer使用RtcInterface基类提供的DTLS、SRTP和SCTP功能
	*  @note 视频帧会被解析为H264格式
	*  @note 音频帧会被解析为OPUS或PCMU格式
	*  @note 支持RTP分片重组，将分片的RTP包重组为完整的媒体帧
	*
	*  上行 NACK（推流侧补包）：
	*  - 服务端作为 RTP **接收方**，若检测到序号空洞，由 `NackGenerator` 生成 RTCP Generic NACK，
	*    请求浏览器/推流端对缺失序号进行 RTX 重传（详见 `share/nack_generator.*`、`docs/producer_nack_design.md`）。
	*  - 视频、音频各维护一份 `NackGenerator`，状态仅在 `RtcService` 的 worker_thread 上更新；
	*    `OnSrtpRtp` 所在网络线程只负责解密与投递任务，避免在 IO 线程做重逻辑。
	*
	*  线程与生命周期：
	*  - `task_safety_`（`PendingTaskSafetyFlag`）用于 PostTask 到 worker 后，Producer 已销毁时丢弃回调，防止 UAF。
	*  - `recv_buffer_` 用于 `OnRecv` 粘包/半包拼装，与网络线程绑定，需注意与 worker 投递的边界。
	*  
	*  使用示例：
	*  @code
	*  auto producer = std::make_shared<RtcProducer>(stream, session);
	*  session->SetProducer(producer);
	*  @endcode
	*/
	class RtcProducer : public RtcInterface,   public Producer, public sigslot::has_slots<>
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于创建RtcProducer实例。它会初始化流对象、会话对象、
		*  SDP对象、DTLS对象、SRTP会话等，并设置音视频相关的参数。
		*  
		*  初始化流程：
		*  1. 调用基类RtcInterface和Producer的构造函数
		*  2. 分配接收缓冲区（recv_buffer_）用于存储接收到的数据
		*  3. 初始化NALU解析器（NalParseInterface）用于解析H264视频
		*  4. 初始化RTCP接收上下文（RtcpContextRecv）用于处理RTCP包
		*  5. 初始化关键帧请求时间戳和流状态
		*  
		*  @param stream 流对象引用，用于推送媒体流，不能为空
		*  @param s 会话对象引用，用于管理会话状态，不能为空
		*  @note 流对象和会话对象通过引用传递，确保生命周期管理正确
		*  @note 接收缓冲区大小默认为8MB，可根据需要调整
		*  
		*  使用示例：
		*  @code
		*  auto producer = std::make_shared<RtcProducer>(stream, session);
		*  @endcode
		*/
		explicit RtcProducer( const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理RtcProducer实例。它会释放所有相关资源，
		*  包括接收缓冲区、NALU解析器、RTCP上下文等。
		*  
		*  清理流程：
		*  1. 释放接收缓冲区内存
		*  2. 释放NALU解析器
		*  3. 释放RTCP接收上下文
		*  4. 调用基类的析构函数进行基类资源清理
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*  @note 流对象和会话对象由外部管理，不需要在此释放
		*/
		virtual ~RtcProducer();

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 接收数据（On Receive）
		*  
		*  该方法用于接收来自客户端的数据。数据可能是DTLS握手数据、
		*  SRTP加密的RTP/RTCP包，或者是STUN/TURN数据包。
		*  
		*  数据处理流程：
		*  1. 将接收到的数据追加到接收缓冲区
		*  2. 检查缓冲区大小，防止溢出
		*  3. 根据数据包类型进行处理：
		*     - DTLS握手数据：传递给DTLS处理
		*     - SRTP加密的RTP包：解密后解析
		*     - SRTP加密的RTCP包：解密后处理
		*     - STUN/TURN数据包：传递给STUN/TURN处理
		*  4. 处理完成后，清理已处理的数据
		*  
		*  数据包类型判断：
		*  - DTLS数据包：前几个字节匹配DTLS记录头
		*  - SRTP数据包：通过SSRC和序列号判断
		*  - STUN数据包：前2字节为0x0001或0x0101
		*  
		*  @param buffer 接收到的数据缓冲区，包含加密或未加密的数据包
		*  @note 该方法会在网络线程中调用，需要注意线程安全
		*  @note 接收缓冲区会自动管理，防止溢出
		*  @note 不同类型的包会被路由到相应的处理函数
		*  
		*  使用示例：
		*  @code
		*  // 在网络线程中调用
		*  producer->OnRecv(received_buffer);
		*  @endcode
		*/
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer)  ;




		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 请求关键帧（Request Key Frame）
		*  
		*  该方法用于请求客户端发送一个关键帧（IDR帧）。当服务器需要快速恢复视频解码时，
		*  可以通过调用此方法请求一个关键帧。对于RTC协议，这是通过发送RTCP PLI（Picture Loss Indication）
		*  或FIR（Full Intra Request）包来实现的。
		*  
		*  关键帧请求流程：
		*  1. 设置请求关键帧的时间戳
		*  2. 生成RTCP PLI或FIR包
		*  3. 通过SRTP加密后发送给客户端
		*  4. 客户端收到请求后，会发送下一个关键帧
		*  
		*  @note 该方法会异步处理，不会阻塞调用线程
		*  @note 对于RTC协议，关键帧请求通过RTCP包实现
		*  @note 关键帧请求可能需要等待一段时间才能收到响应
		*  
		*  使用场景：
		*  - 新消费者加入时，需要快速获取关键帧以开始播放
		*  - 网络丢包导致解码失败时，需要请求关键帧恢复
		*  - 视频质量切换时，需要请求关键帧重新编码
		*  
		*  使用示例：
		*  @code
		*  producer->RequestKeyFrame();
		*  @endcode
		*/
		virtual void RequestKeyFrame();

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 设置流的转发状态（Set Stream Status）
		*  
		*  该方法用于控制流的转发行为。当没有用户观看时，可以停止转发以节省资源；
		*  当有用户观看时，自动恢复转发，确保用户可以正常观看。
		*  
		*  @param status 是否转发流，true表示启用转发，false表示停止转发
		*  @note 该方法会更新内部流状态标志，影响后续的数据处理
		*  
		*  使用示例：
		*  @code
		*  producer->SetStreamStatus(false); // 停止转发
		*  producer->SetStreamStatus(true);  // 恢复转发
		*  @endcode
		*/
		virtual  void SetStreamStatus(bool status);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 定时器回调（On Timer）
		*  
		*  该方法用于定时器回调，定期执行一些维护任务，如发送RTCP接收报告、
		*  检查关键帧请求状态等。
		*  
		*  定时任务：
		*  1. 发送RTCP接收报告（Receiver Report）
		*  2. 检查是否需要请求关键帧
		*  3. 更新统计信息
		*  
		*  @note 该方法会被定时器定期调用
		*  @note 定时器间隔通常为1秒
		*/
		void OnTimer();
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取资源类型（Get Share Resource Type）
		*  
		*  该方法用于标识该生产者的资源类型。对于RtcProducer，返回kProducerTypeRtc。
		*  
		*  @return 返回kProducerTypeRtc，表示这是RTC协议的生产者
		*/
		virtual ShareResourceType ShareResouceType() const { return kProducerTypeRtc; }
	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 处理SDP Offer（Process Offer SDP）
		*  
		*  该方法用于处理客户端发送的SDP Offer。它会解析SDP内容，提取音频/视频编解码器、
		*  ICE候选、DTLS指纹等信息，并准备生成SDP Answer。
		*  
		*  处理流程：
		*  1. 解析SDP Offer，提取会话信息
		*  2. 提取音频/视频媒体描述（m=行）
		*  3. 提取ICE候选（candidate）
		*  4. 提取DTLS指纹（fingerprint）
		*  5. 提取用户名片段（ufrag）和密码（pwd）
		*  6. 根据提取的信息准备生成Answer
		*  
		*  @param rtc_sdp_type SDP类型，kRtcSdpPush表示推流，kRtcSdpPlay表示拉流
		*  @param sdp SDP Offer的字符串内容
		*  @return 如果处理成功返回true，否则返回false
		*  @note 该方法会验证SDP格式和内容
		*  @note 处理成功后，可以通过BuildAnswerSdp()生成Answer
		*  
		*  使用示例：
		*  @code
		*  std::string sdp_offer = "..."; // SDP Offer内容
		*  if (producer->ProcessOfferSdp(libmedia_transfer_protocol::librtc::kRtcSdpPush, sdp_offer)) {
		*      std::string answer = producer->BuildAnswerSdp();
		*  }
		*  @endcode
		*/
		virtual bool ProcessOfferSdp(libmedia_transfer_protocol::librtc::RtcSdpType  rtc_sdp_type, const std::string& sdp);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取本地用户名片段（Get Local Username Fragment）
		*  
		*  该方法用于获取本地生成的ICE用户名片段（ufrag）。用户名片段用于ICE连接建立。
		*  
		*  @return 返回本地用户名片段的常量引用
		*  @note 用户名片段在SDP处理时自动生成
		*/
		virtual const std::string& LocalUFrag() const;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取本地密码（Get Local Password）
		*  
		*  该方法用于获取本地生成的ICE密码（pwd）。密码用于ICE连接建立。
		*  
		*  @return 返回本地密码的常量引用
		*  @note 密码长度在12-32字节之间
		*  @note 密码在SDP处理时自动生成
		*/
		virtual const std::string& LocalPasswd() const;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 获取远程用户名片段（Get Remote Username Fragment）
		*  
		*  该方法用于获取从SDP Offer中提取的远程用户名片段（ufrag）。
		*  
		*  @return 返回远程用户名片段的常量引用
		*  @note 远程用户名片段从SDP Offer中提取
		*/
		virtual const std::string& RemoteUFrag() const;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 构建SDP Answer（Build Answer SDP）
		*  
		*  该方法用于根据处理后的SDP Offer生成SDP Answer。Answer包含服务器选择的
		*  编解码器、ICE候选、DTLS指纹等信息。
		*  
		*  Answer生成流程：
		*  1. 创建SDP会话描述
		*  2. 添加音频/视频媒体描述，选择支持的编解码器
		*  3. 添加ICE候选（服务器地址和端口）
		*  4. 添加DTLS指纹和证书信息
		*  5. 添加用户名片段和密码
		*  6. 生成完整的SDP Answer字符串
		*  
		*  @return 返回生成的SDP Answer字符串
		*  @note 必须先调用ProcessOfferSdp()处理Offer
		*  @note Answer会根据Offer中的编解码器选择支持的格式
		*  
		*  使用示例：
		*  @code
		*  std::string answer = producer->BuildAnswerSdp();
		*  // 将answer发送给客户端
		*  @endcode
		*/
		virtual std::string BuildAnswerSdp();

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 开始DTLS握手（May Run DTLS）
		*  
		*  该方法用于启动DTLS握手过程。根据SDP中的信息，决定作为DTLS客户端还是
		*  服务器端发起握手。DTLS握手用于交换密钥，建立SRTP加密会话。
		*  
		*  DTLS角色选择：
		*  - 如果SDP中设置了"setup:actpass"，服务器通常作为服务器端
		*  - 如果SDP中设置了"setup:active"，服务器作为客户端
		*  - 如果SDP中设置了"setup:passive"，服务器作为服务器端
		*  
		*  握手流程：
		*  1. 确定DTLS角色（客户端或服务器端）
		*  2. 加载DTLS证书和私钥
		*  3. 创建DTLS对象并连接信号
		*  4. 开始握手过程
		*  
		*  @note 该方法会在SDP处理完成后调用
		*  @note DTLS握手成功后，会触发OnDtlsConnected回调
		*  @note DTLS握手失败后，会触发OnDtlsFailed回调
		*  
		*  使用示例：
		*  @code
		*  producer->MayRunDtls();
		*  @endcode
		*/
		virtual void MayRunDtls();


		//virtual void RemoveGlobalData() override;
	public:

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS 明文回调：收到对端 DTLS 记录层数据（OnDtlsRecv）
		*
		*  在 DTLS 握手或应用数据阶段，底层将解密前/协商中的 DTLS 记录交给 `Dtls` 状态机处理。
		*  通常由 `RtcInterface` / `RtcServer` 在解析 UDP 载荷后根据内容类型路由到此。
		*
		*  @param buf DTLS 记录字节流起始指针
		*  @param size 字节长度
		*  @note 调用线程一般为 RTC 网络/工作线程，勿在回调内长时间阻塞
		*/
		virtual void OnDtlsRecv(const uint8_t* buf, size_t size);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief SRTP 解密后的 RTP 媒体包入口（OnSrtpRtp）
		*
		*  处理流程概要：
		*  1. SRTP 解密成功后得到 RTP payload，解析 SSRC、seq、PT、扩展（如 TWCC transport-wide seq）；
		*  2. 视频路径：NAL 解析、拼帧，回调 `Session`/`Stream` 推送编码帧；
		*  3. 若启用 NACK：将 `{seq, media_ssrc, now_ms}` Post 到 worker_thread，在 `ProcessIncomingSeqOnWorker`
		*     中驱动 `NackGenerator` 并可能 `SendSrtpRtcp` 发出 RTCP NACK；
		*  4. 非视频/非协商 SSRC 的包应过滤，避免误入 NACK 与统计。
		*
		*  @param data 已解密 RTP 缓冲区（可能被原地修改，取决于 SRTP API）
		*  @param size 缓冲区长度
		*  @note 热点路径：应避免在此处做重计算；与 `ProcessIncomingSeqOnWorker` 分工见实现文件
		*/
		virtual void OnSrtpRtp(  uint8_t* data, size_t size);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief SRTP 解密后的 RTCP 控制包入口（OnSrtpRtcp）
		*
		*  典型处理：RR、SDES、TransportFeedback（TWCC）、PLI/FIR 等。用于更新 RTT、丢包统计、
		*  `RtcpContextRecv` 内部状态等（具体字段以 rtc_producer.cpp 为准）。
		*
		*  @param data 已解密 RTCP 复合包缓冲区
		*  @param size 长度
		*  @note 与 `OnSrtpRtp` 相同，一般在网络线程调用；复合包内需循环解析多个 RTCP chunk
		*/
		virtual void OnSrtpRtcp(  uint8_t* data, size_t size);
	public:
		//virtual void OnVideoFrame(const libmedia_codec::EncodedImage& frame);
		//virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts);

	public:


	public:
		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS：开始握手/协商（OnDtlsConnecting）
		*  @param dtls 当前关联的 `Dtls` 实例指针
		*  @note 可在此打日志或更新 UI 状态；密钥尚未就绪
		*/
		virtual void OnDtlsConnecting(libmedia_transfer_protocol::libssl::Dtls* dtls);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS：握手成功，导出 SRTP 密钥（OnDtlsConnected）
		*
		*  使用协商出的 `srtpLocalKey` / `srtpRemoteKey` 初始化 `SrtpSession`，之后方可 `DecryptRtp`/`EncryptRtcp`。
		*
		*  @param dtls DTLS 实例
		*  @param srtpCryptoSuite 选中的 SRTP 加密套件
		*  @param srtpLocalKey 本端导出密钥材料
		*  @param srtpLocalKeyLen 本端密钥长度
		*  @param srtpRemoteKey 对端导出密钥材料
		*  @param srtpRemoteKeyLen 对端密钥长度
		*  @param remote_cert 对端证书 PEM（输出/填充由实现约定）
		*/
		virtual void OnDtlsConnected(libmedia_transfer_protocol::libssl::Dtls* dtls,
			libmedia_transfer_protocol::libsrtp::CryptoSuite srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remote_cert);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS：需要向对端发送握手/告警等记录（OnDtlsSendPakcet）
		*  @param dtls DTLS 实例
		*  @param data 待发送的 DTLS 记录明文或密文（依实现）
		*  @param len 长度
		*  @note 实现中应通过 ICE 将数据发到当前选中的远端 `rtc_remote_address_`
		*/
		virtual void OnDtlsSendPakcet(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS：连接正常关闭（OnDtlsClosed）
		*  @param dtls DTLS 实例
		*/
		virtual void OnDtlsClosed(libmedia_transfer_protocol::libssl::Dtls* dtls);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS：握手或验证失败（OnDtlsFailed）
		*  @param dtls DTLS 实例
		*  @note 应停止媒体收发并通知上层 Session 失败
		*/
		virtual void OnDtlsFailed(libmedia_transfer_protocol::libssl::Dtls* dtls);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief DTLS：收到应用层数据（SCTP 建立前极少使用）（OnDtlsApplicationDataReceived）
		*  @param dtls DTLS 实例
		*  @param data 应用数据指针
		*  @param len 长度
		*/
		virtual void OnDtlsApplicationDataReceived(libmedia_transfer_protocol::libssl::Dtls* dtls, const uint8_t* data, size_t len);

	public:

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 数据通道：本端主动或被动打开流时的消息回调（OnDataChannel）
		*
		*  @param params SCTP 流参数（流 id、可靠性等）
		*  @param ppid SCTP Payload Protocol Identifier
		*  @param msg 消息体
		*  @param len 消息长度
		*/
		virtual  void OnDataChannel(
			const  libmedia_transfer_protocol::librtc::SctpStreamParameters& params,
			uint32_t ppid, const uint8_t* msg, size_t len);

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief SCTP：收到对端 DATA chunk 组装后的用户消息（OnSctpAssociationMessageReceived）
		*
		*  @param sctpAssociation SCTP 关联指针
		*  @param streamId SCTP 流 ID
		*  @param ppid 负载协议标识
		*  @param msg 消息缓冲区
		*  @param len 长度
		*/
		virtual void OnSctpAssociationMessageReceived(
			libmedia_transfer_protocol::librtc::SctpAssociation* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len) override;
	public:
		//virtual ShareResourceType ShareResouceType() const { return kConsumerTypeRTC; }

	private:
	//	static std::string GetUFrag(int size);
	//	static uint32_t GetSsrc(int size);
	private:
		//std::string local_ufrag_;
		//std::string local_passwd_;  //[12, 32]
		//libmedia_transfer_protocol::librtc::RtcSdp sdp_;
		////Dtls dtls_;
		//
		////libmedia_transfer_protocol::librtc::DtlsCerts   dtls_certs_;
		//libmedia_transfer_protocol::libssl::Dtls   dtls_;
		//
		//bool dtls_done_{ false };
		//
		////rtc::SocketAddress             remote_address_;
		//libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_send_session_;
		//libmedia_transfer_protocol::libsrtp::SrtpSession* srtp_recv_session_;
		//
		//
		//
		//
		//uint32_t      audio_seq_ = 100;
		//uint32_t      video_seq_ = 100;
		//libmedia_transfer_protocol::RtpHeaderExtensionMap     rtp_header_extension_map_;



	private:

		//std::unique_ptr< libmedia_transfer_protocol::libmpeg::MpegDecoder>    mpeg_decoder_;

		/**
		*  @brief 异步任务安全标志（libwebrtc PendingTaskSafetyFlag）
		*
		*  `RtcProducer` 析构时置位，worker_thread 上已 Post 的 lambda 执行前检查，
		*  避免访问已释放的 `this`（UAF 防护）。
		*/
		const rtc::scoped_refptr<webrtc::PendingTaskSafetyFlag> task_safety_;

		/**
		*  @brief 接收侧拼包缓冲区（与 recv_buffer_size_ 成对）
		*
		*  `OnRecv` 可能多次收到同一 DTLS/SRTP 片段或一次 UDP 多包，需累积后再解析。
		*/
		std::unique_ptr<uint8_t[]> recv_buffer_;
		/// `recv_buffer_` 分配的总字节数；写入游标见 .cpp
		int32_t recv_buffer_size_;

		/**
		*  @brief H.264 NAL 解析与拼帧（NalParseFactory 创建具体实现）
		*/
		std::unique_ptr<libmedia_codec::NalParseInterface>  nal_parse_;

		/**
		*  @brief RTCP 接收上下文（RR、扩展报告等，与带宽/RTT 统计联动）
		*/
		std::unique_ptr<libmedia_transfer_protocol::librtcp::RtcpContextRecv>   rtcp_context_recv_;

		/**
		*  @brief 因解析失败等跳过的 RTP/RTCP 包计数（可观测性）
		*/
		int32_t   num_skipped_packets_ = 0;

		/**
		*  @brief 最近一次 RR 相关时间戳（毫秒，语义以 .cpp 为准）
		*/
		int64_t    rtcp_rr_timestamp_;

		/**
		*  @brief 最近一次请求关键帧的时间戳（毫秒），用于节流 PLI/FIR
		*/
		int64_t     request_key_frame_{0};

		/**
		*  @brief 是否继续向 Stream 转发解析后的帧（无观众时可 false 省资源）
		*/
		bool       stream_status_ = true;

		/**
		*  @author chensong
		*  @date 2025-10-18
		*  @brief 推流端统计（码率、帧数、NACK 次数等）
		*/
		std::shared_ptr<ProducerStatistics> statistics_;

		/**
		*  @brief 视频 RTP 序号空洞检测与 RTCP Generic NACK（见 producer_nack_design.md）
		*
		*  仅在 worker_thread 上操作状态；网络线程只投递任务。
		*/
		std::unique_ptr<NackGenerator>  video_nack_generator_;

		/**
		*  @brief 音频 RTP NACK 生成器（与视频 SSRC 空间分离）
		*/
		std::unique_ptr<NackGenerator>  audio_nack_generator_;

		/**
		*  @brief 是否启用上行 NACK（默认 true；可接 YAML）
		*/
		bool  nack_enabled_ = true;

		/**
		*  @author chensong
		*  @date 2025-11-12
		*  @brief 在 worker_thread 上处理一次 RTP 到达：更新 NackGenerator，必要时发送 RTCP NACK
		*
		*  @param is_video true=视频 SSRC，false=音频 SSRC
		*  @param seq RTP 16 位序号
		*  @param media_ssrc 填入 RTCP NACK 的 media_ssrc
		*  @param now_ms 网络线程采样时间，用于节流与冷却
		*  @note 仅 worker_thread 调用；内部可能 `SendSrtpRtcp`
		*/
		void ProcessIncomingSeqOnWorker(bool is_video,
		                                uint16_t seq,
		                                uint32_t media_ssrc,
		                                int64_t now_ms);
	};
}


#endif // 