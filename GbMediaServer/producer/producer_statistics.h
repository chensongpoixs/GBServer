/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#ifndef _GB_MEDIA_SERVER_PRODUCER_STATISTICS_H_
#define _GB_MEDIA_SERVER_PRODUCER_STATISTICS_H_

#include "share/statistics_base.h"
#include <atomic>
#include <string>

namespace gb_media_server {

/**
 * @author chensong
 * @date 2025-10-18
 * @brief Producer统计数据类（Producer Statistics）
 * 
 * 用于收集和管理Producer的统计数据，包括RTP接收、RTCP、DTLS等。
 */
class ProducerStatistics : public StatisticsBase {
public:
    ProducerStatistics(const std::string& session_name, 
                      const std::string& stream_name,
                      const std::string& producer_type = "rtc");
    virtual ~ProducerStatistics();
    
    // 实现基类接口
    virtual std::string ToJson() const override;
    virtual void Reset() override;
    
    // RTP包接收统计
    void OnRtpPacketReceived(bool is_video, size_t packet_size);
    void OnRtpPacketLost(bool is_video, uint32_t lost_count);
    
    // 帧统计
    void OnFrameReceived(bool is_video, bool is_key_frame);
    void OnFrameDecoded(bool is_video);
    void OnFrameDropped(bool is_video);
    
    // RTCP统计
    void OnRtcpSRReceived();
    void OnRtcpRRSent();
    void OnNackReceived(uint32_t count);
    void OnPliReceived();
    void OnFirReceived();
    void UpdateRtt(double rtt_ms);
    void UpdateJitter(bool is_video, double jitter_ms);
    
    // DTLS统计
    void OnDtlsHandshakeStart();
    void OnDtlsHandshakeComplete(const std::string& dtls_cipher, const std::string& srtp_cipher);
    void OnDtlsHandshakeFailed();
    
    // 设置媒体信息
    void SetVideoInfo(uint32_t ssrc, uint32_t width, uint32_t height, const std::string& codec);
    void SetAudioInfo(uint32_t ssrc, uint32_t sample_rate, uint32_t channels, const std::string& codec);
    
    // 设置状态
    void SetState(const std::string& state);
    
    // 定期更新（计算派生指标）
    void Update();
    
    // 获取会话名称
    const std::string& GetSessionName() const { return session_name_; }
    
    // 获取统计数据的getter方法（Getter methods for statistics data）
    // @author chensong
    // @date 2025-10-18
    uint64_t GetVideoBytesReceived() const { return video_stats_.bytes_received.load(); }
    uint64_t GetAudioBytesReceived() const { return audio_stats_.bytes_received.load(); }
    uint64_t GetVideoPacketsReceived() const { return video_stats_.packets_received.load(); }
    uint64_t GetAudioPacketsReceived() const { return audio_stats_.packets_received.load(); }
    uint32_t GetVideoFramesReceived() const { return video_stats_.frames_received.load(); }
    uint32_t GetNackCount() const { return rtcp_stats_.nack_count.load(); }
    uint32_t GetPliCount() const { return rtcp_stats_.pli_count.load(); }
    double GetVideoBitrate() const { return video_stats_.bitrate; }
    double GetAudioBitrate() const { return audio_stats_.bitrate; }
    double GetVideoPacketLossRate() const { return video_stats_.packet_loss_rate; }
    double GetRtt() const { return rtcp_stats_.rtt; }
    double GetVideoJitter() const { return video_stats_.jitter; }
    double GetVideoFps() const { return video_stats_.fps; }
    
private:
    // 计算码率
    void CalculateBitrate();
    
    // 计算丢包率
    void CalculatePacketLossRate();
    
    // 计算帧率
    void CalculateFps();
    
private:
    std::string session_name_;
    std::string stream_name_;
    std::string producer_type_;
    std::string state_;
    
    // 视频统计
    struct VideoStats {
        uint32_t ssrc = 0;
        std::atomic<uint64_t> packets_received{0};
        std::atomic<uint64_t> bytes_received{0};
        std::atomic<uint32_t> packets_lost{0};
        std::atomic<uint32_t> frames_received{0};
        std::atomic<uint32_t> key_frames_received{0};
        std::atomic<uint32_t> frames_decoded{0};
        std::atomic<uint32_t> frames_dropped{0};
        uint32_t width = 0;
        uint32_t height = 0;
        std::string codec;
        double jitter = 0.0;
        double bitrate = 0.0;
        double fps = 0.0;
        double packet_loss_rate = 0.0;
    } video_stats_;
    
    // 音频统计
    struct AudioStats {
        uint32_t ssrc = 0;
        std::atomic<uint64_t> packets_received{0};
        std::atomic<uint64_t> bytes_received{0};
        std::atomic<uint32_t> packets_lost{0};
        uint32_t sample_rate = 0;
        uint32_t channels = 0;
        std::string codec;
        double jitter = 0.0;
        double bitrate = 0.0;
        double packet_loss_rate = 0.0;
    } audio_stats_;
    
    // RTCP统计
    struct RtcpStats {
        std::atomic<uint32_t> sr_count{0};
        std::atomic<uint32_t> rr_count{0};
        std::atomic<uint32_t> nack_count{0};
        std::atomic<uint32_t> pli_count{0};
        std::atomic<uint32_t> fir_count{0};
        double rtt = 0.0;
    } rtcp_stats_;
    
    // DTLS统计
    struct SecurityStats {
        std::string state = "idle";
        std::string dtls_cipher;
        std::string srtp_cipher;
        std::string local_fingerprint;
        std::string remote_fingerprint;
        int64_t handshake_start_time = 0;
        int64_t handshake_duration = 0;
    } security_stats_;
    
    // 用于计算码率的临时变量
    int64_t last_bitrate_calc_time_ = 0;
    uint64_t last_video_bytes_ = 0;
    uint64_t last_audio_bytes_ = 0;
    
    // 用于计算帧率的临时变量
    int64_t last_fps_calc_time_ = 0;
    uint32_t last_video_frames_ = 0;
};

} // namespace gb_media_server

#endif // _GB_MEDIA_SERVER_PRODUCER_STATISTICS_H_
