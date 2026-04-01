/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#ifndef _GB_MEDIA_SERVER_CONSUMER_STATISTICS_H_
#define _GB_MEDIA_SERVER_CONSUMER_STATISTICS_H_

#include "share/statistics_base.h"
#include <atomic>
#include <string>

namespace gb_media_server {

/**
 * @author chensong
 * @date 2025-10-18
 * @brief Consumer统计数据类（Consumer Statistics）
 * 
 * 用于收集和管理Consumer的统计数据，包括RTP发送、RTCP、质量评估等。
 */
class ConsumerStatistics : public StatisticsBase {
public:
    ConsumerStatistics(const std::string& consumer_id,
                      const std::string& session_name,
                      const std::string& stream_name,
                      const std::string& consumer_type);
    virtual ~ConsumerStatistics();
    
    // 实现基类接口
    virtual std::string ToJson() const override;
    virtual void Reset() override;
    
    // RTP包发送统计
    void OnRtpPacketSent(bool is_video, size_t packet_size);
    void OnRtxPacketSent(size_t packet_size);
    
    // 帧统计
    void OnFrameSent(bool is_video, bool is_key_frame);
    void OnFrameEncoded(bool is_video);
    
    // RTCP统计
    void OnRtcpSRSent();
    void OnRtcpRRReceived(uint32_t packets_lost, double jitter);
    void OnNackSent(uint32_t count);
    void OnPliSent();
    void OnFirSent();
    void UpdateRtt(double rtt_ms);
    
    // 设置媒体信息
    void SetVideoInfo(uint32_t ssrc, uint32_t rtx_ssrc, uint32_t width, 
                     uint32_t height, const std::string& codec);
    void SetAudioInfo(uint32_t ssrc, const std::string& codec);
    
    // 设置远程地址
    void SetRemoteAddress(const std::string& address);
    
    // 设置状态
    void SetState(const std::string& state);
    
    // 定期更新
    void Update();
    
    // 获取Consumer ID
    const std::string& GetConsumerId() const { return consumer_id_; }
    
private:
    void CalculateBitrate();
    void CalculatePacketLossRate();
    void CalculateFps();
    void CalculateQualityScore();
    
private:
    std::string consumer_id_;
    std::string session_name_;
    std::string stream_name_;
    std::string consumer_type_;
    std::string remote_address_;
    std::string state_;
    
    // 视频统计
    struct VideoStats {
        uint32_t ssrc = 0;
        uint32_t rtx_ssrc = 0;
        std::atomic<uint64_t> packets_sent{0};
        std::atomic<uint64_t> bytes_sent{0};
        std::atomic<uint64_t> rtx_packets_sent{0};
        std::atomic<uint64_t> rtx_bytes_sent{0};
        std::atomic<uint32_t> packets_lost{0};
        std::atomic<uint32_t> frames_sent{0};
        std::atomic<uint32_t> key_frames_sent{0};
        std::atomic<uint32_t> frames_encoded{0};
        std::atomic<uint32_t> nack_count{0};
        std::atomic<uint32_t> pli_count{0};
        std::atomic<uint32_t> fir_count{0};
        uint32_t width = 0;
        uint32_t height = 0;
        std::string codec;
        double bitrate = 0.0;
        double fps = 0.0;
        double packet_loss_rate = 0.0;
    } video_stats_;
    
    // 音频统计
    struct AudioStats {
        uint32_t ssrc = 0;
        std::atomic<uint64_t> packets_sent{0};
        std::atomic<uint64_t> bytes_sent{0};
        std::atomic<uint32_t> packets_lost{0};
        std::string codec;
        double bitrate = 0.0;
        double packet_loss_rate = 0.0;
    } audio_stats_;
    
    // RTCP统计
    struct RtcpStats {
        std::atomic<uint32_t> sr_count{0};
        std::atomic<uint32_t> rr_count{0};
        double rtt = 0.0;
        double jitter = 0.0;
    } rtcp_stats_;
    
    // 质量评估
    struct QualityMetrics {
        double mos_score = 0.0;
        std::string quality_level = "unknown";
        double video_quality_score = 0.0;
        double audio_quality_score = 0.0;
    } quality_metrics_;
    
    // 临时变量
    int64_t last_bitrate_calc_time_ = 0;
    uint64_t last_video_bytes_ = 0;
    uint64_t last_audio_bytes_ = 0;
    int64_t last_fps_calc_time_ = 0;
    uint32_t last_video_frames_ = 0;
};

} // namespace gb_media_server

#endif // _GB_MEDIA_SERVER_CONSUMER_STATISTICS_H_
