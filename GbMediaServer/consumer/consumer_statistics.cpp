/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/

#include "consumer/consumer_statistics.h"
#include "utils/json_utils.h"
#include <algorithm>
#include <cmath>

namespace gb_media_server {

ConsumerStatistics::ConsumerStatistics(const std::string& consumer_id,
                                       const std::string& session_name,
                                       const std::string& stream_name,
                                       const std::string& consumer_type)
    : StatisticsBase()
    , consumer_id_(consumer_id)
    , session_name_(session_name)
    , stream_name_(stream_name)
    , consumer_type_(consumer_type)
    , state_("connecting")
    , last_bitrate_calc_time_(0)
    , last_video_bytes_(0)
    , last_audio_bytes_(0)
    , last_fps_calc_time_(0)
    , last_video_frames_(0)
{
}

ConsumerStatistics::~ConsumerStatistics() {
}

void ConsumerStatistics::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    StatisticsBase::Reset();
    
    // 重置视频统计
    video_stats_.packets_sent = 0;
    video_stats_.bytes_sent = 0;
    video_stats_.rtx_packets_sent = 0;
    video_stats_.rtx_bytes_sent = 0;
    video_stats_.packets_lost = 0;
    video_stats_.frames_sent = 0;
    video_stats_.key_frames_sent = 0;
    video_stats_.frames_encoded = 0;
    video_stats_.nack_count = 0;
    video_stats_.pli_count = 0;
    video_stats_.fir_count = 0;
    video_stats_.bitrate = 0.0;
    video_stats_.fps = 0.0;
    video_stats_.packet_loss_rate = 0.0;
    
    // 重置音频统计
    audio_stats_.packets_sent = 0;
    audio_stats_.bytes_sent = 0;
    audio_stats_.packets_lost = 0;
    audio_stats_.bitrate = 0.0;
    audio_stats_.packet_loss_rate = 0.0;
    
    // 重置RTCP统计
    rtcp_stats_.sr_count = 0;
    rtcp_stats_.rr_count = 0;
    rtcp_stats_.rtt = 0.0;
    rtcp_stats_.jitter = 0.0;
    
    // 重置质量评估
    quality_metrics_.mos_score = 0.0;
    quality_metrics_.quality_level = "unknown";
    quality_metrics_.video_quality_score = 0.0;
    quality_metrics_.audio_quality_score = 0.0;
    
    // 重置临时变量
    last_bitrate_calc_time_ = 0;
    last_video_bytes_ = 0;
    last_audio_bytes_ = 0;
    last_fps_calc_time_ = 0;
    last_video_frames_ = 0;
}

void ConsumerStatistics::OnRtpPacketSent(bool is_video, size_t packet_size) {
    if (is_video) {
        video_stats_.packets_sent++;
        video_stats_.bytes_sent += packet_size;
    } else {
        audio_stats_.packets_sent++;
        audio_stats_.bytes_sent += packet_size;
    }
    UpdateTimestamp();
}

void ConsumerStatistics::OnRtxPacketSent(size_t packet_size) {
    video_stats_.rtx_packets_sent++;
    video_stats_.rtx_bytes_sent += packet_size;
    UpdateTimestamp();
}

void ConsumerStatistics::OnFrameSent(bool is_video, bool is_key_frame) {
    if (is_video) {
        video_stats_.frames_sent++;
        if (is_key_frame) {
            video_stats_.key_frames_sent++;
        }
    }
    UpdateTimestamp();
}

void ConsumerStatistics::OnFrameEncoded(bool is_video) {
    if (is_video) {
        video_stats_.frames_encoded++;
    }
}

void ConsumerStatistics::OnRtcpSRSent() {
    rtcp_stats_.sr_count++;
}

void ConsumerStatistics::OnRtcpRRReceived(uint32_t packets_lost, double jitter) {
    rtcp_stats_.rr_count++;
    video_stats_.packets_lost += packets_lost;
    
    std::lock_guard<std::mutex> lock(mutex_);
    rtcp_stats_.jitter = jitter;
}

void ConsumerStatistics::OnNackSent(uint32_t count) {
    video_stats_.nack_count += count;
}

void ConsumerStatistics::OnPliSent() {
    video_stats_.pli_count++;
}

void ConsumerStatistics::OnFirSent() {
    video_stats_.fir_count++;
}

void ConsumerStatistics::UpdateRtt(double rtt_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    rtcp_stats_.rtt = rtt_ms;
}

void ConsumerStatistics::SetVideoInfo(uint32_t ssrc, uint32_t rtx_ssrc, 
                                     uint32_t width, uint32_t height, 
                                     const std::string& codec) {
    std::lock_guard<std::mutex> lock(mutex_);
    video_stats_.ssrc = ssrc;
    video_stats_.rtx_ssrc = rtx_ssrc;
    video_stats_.width = width;
    video_stats_.height = height;
    video_stats_.codec = codec;
}

void ConsumerStatistics::SetAudioInfo(uint32_t ssrc, const std::string& codec) {
    std::lock_guard<std::mutex> lock(mutex_);
    audio_stats_.ssrc = ssrc;
    audio_stats_.codec = codec;
}

void ConsumerStatistics::SetRemoteAddress(const std::string& address) {
    std::lock_guard<std::mutex> lock(mutex_);
    remote_address_ = address;
}

void ConsumerStatistics::SetState(const std::string& state) {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = state;
}

void ConsumerStatistics::Update() {
    CalculateBitrate();
    CalculatePacketLossRate();
    CalculateFps();
    CalculateQualityScore();
}

void ConsumerStatistics::CalculateBitrate() {
    int64_t current_time = GetCurrentTimeMs();
    
    if (last_bitrate_calc_time_ == 0) {
        last_bitrate_calc_time_ = current_time;
        last_video_bytes_ = video_stats_.bytes_sent.load();
        last_audio_bytes_ = audio_stats_.bytes_sent.load();
        return;
    }
    
    int64_t time_diff = current_time - last_bitrate_calc_time_;
    if (time_diff < 1000) {
        return;
    }
    
    uint64_t current_video_bytes = video_stats_.bytes_sent.load();
    uint64_t current_audio_bytes = audio_stats_.bytes_sent.load();
    
    // 计算视频码率（kbps）
    uint64_t video_bytes_diff = current_video_bytes - last_video_bytes_;
    video_stats_.bitrate = (video_bytes_diff * 8.0 * 1000.0) / (time_diff * 1024.0);
    
    // 计算音频码率（kbps）
    uint64_t audio_bytes_diff = current_audio_bytes - last_audio_bytes_;
    audio_stats_.bitrate = (audio_bytes_diff * 8.0 * 1000.0) / (time_diff * 1024.0);
    
    last_bitrate_calc_time_ = current_time;
    last_video_bytes_ = current_video_bytes;
    last_audio_bytes_ = current_audio_bytes;
}

void ConsumerStatistics::CalculatePacketLossRate() {
    // 计算视频丢包率
    uint64_t video_total = video_stats_.packets_sent.load() + video_stats_.packets_lost.load();
    if (video_total > 0) {
        video_stats_.packet_loss_rate = (video_stats_.packets_lost.load() * 100.0) / video_total;
    }
    
    // 计算音频丢包率
    uint64_t audio_total = audio_stats_.packets_sent.load() + audio_stats_.packets_lost.load();
    if (audio_total > 0) {
        audio_stats_.packet_loss_rate = (audio_stats_.packets_lost.load() * 100.0) / audio_total;
    }
}

void ConsumerStatistics::CalculateFps() {
    int64_t current_time = GetCurrentTimeMs();
    
    if (last_fps_calc_time_ == 0) {
        last_fps_calc_time_ = current_time;
        last_video_frames_ = video_stats_.frames_sent.load();
        return;
    }
    
    int64_t time_diff = current_time - last_fps_calc_time_;
    if (time_diff < 1000) {
        return;
    }
    
    uint32_t current_frames = video_stats_.frames_sent.load();
    uint32_t frames_diff = current_frames - last_video_frames_;
    
    video_stats_.fps = (frames_diff * 1000.0) / time_diff;
    
    last_fps_calc_time_ = current_time;
    last_video_frames_ = current_frames;
}

void ConsumerStatistics::CalculateQualityScore() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 基于丢包率、RTT、抖动计算质量评分
    double packet_loss_impact = 0.0;
    double rtt_impact = 0.0;
    double jitter_impact = 0.0;
    
    // 丢包率影响（0-30分）
    if (video_stats_.packet_loss_rate < 0.5) {
        packet_loss_impact = 30.0;
    } else if (video_stats_.packet_loss_rate < 1.0) {
        packet_loss_impact = 25.0;
    } else if (video_stats_.packet_loss_rate < 3.0) {
        packet_loss_impact = 20.0;
    } else if (video_stats_.packet_loss_rate < 5.0) {
        packet_loss_impact = 10.0;
    } else {
        packet_loss_impact = 0.0;
    }
    
    // RTT影响（0-30分）
    if (rtcp_stats_.rtt < 100.0) {
        rtt_impact = 30.0;
    } else if (rtcp_stats_.rtt < 200.0) {
        rtt_impact = 25.0;
    } else if (rtcp_stats_.rtt < 400.0) {
        rtt_impact = 20.0;
    } else if (rtcp_stats_.rtt < 600.0) {
        rtt_impact = 10.0;
    } else {
        rtt_impact = 0.0;
    }
    
    // 抖动影响（0-20分）
    if (rtcp_stats_.jitter < 30.0) {
        jitter_impact = 20.0;
    } else if (rtcp_stats_.jitter < 50.0) {
        jitter_impact = 15.0;
    } else if (rtcp_stats_.jitter < 100.0) {
        jitter_impact = 10.0;
    } else {
        jitter_impact = 0.0;
    }
    
    // 帧率影响（0-20分）
    double fps_impact = 0.0;
    if (video_stats_.fps >= 30.0) {
        fps_impact = 20.0;
    } else if (video_stats_.fps >= 25.0) {
        fps_impact = 15.0;
    } else if (video_stats_.fps >= 15.0) {
        fps_impact = 10.0;
    } else {
        fps_impact = 0.0;
    }
    
    // 计算视频质量评分（0-100）
    quality_metrics_.video_quality_score = packet_loss_impact + rtt_impact + 
                                          jitter_impact + fps_impact;
    
    // 音频质量评分（简化版）
    quality_metrics_.audio_quality_score = std::min(100.0, 
        100.0 - audio_stats_.packet_loss_rate * 10.0);
    
    // 计算MOS评分（1-5）
    double avg_score = (quality_metrics_.video_quality_score + 
                       quality_metrics_.audio_quality_score) / 2.0;
    quality_metrics_.mos_score = 1.0 + (avg_score / 100.0) * 4.0;
    
    // 确定质量等级
    if (quality_metrics_.mos_score >= 4.0) {
        quality_metrics_.quality_level = "excellent";
    } else if (quality_metrics_.mos_score >= 3.5) {
        quality_metrics_.quality_level = "good";
    } else if (quality_metrics_.mos_score >= 3.0) {
        quality_metrics_.quality_level = "fair";
    } else {
        quality_metrics_.quality_level = "poor";
    }
}

std::string ConsumerStatistics::ToJson() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 构建基础信息
    JsonBuilder basic_info;
    basic_info.AddString("consumer_id", consumer_id_);
    basic_info.AddString("session_name", session_name_);
    basic_info.AddString("stream_name", stream_name_);
    basic_info.AddString("consumer_type", consumer_type_);
    basic_info.AddString("remote_address", remote_address_);
    basic_info.AddNumber("created_time", created_time_);
    basic_info.AddNumber("duration", GetDuration());
    basic_info.AddString("state", state_);
    
    // 构建视频统计
    JsonBuilder video_json;
    video_json.AddNumber("ssrc", video_stats_.ssrc);
    video_json.AddNumber("rtx_ssrc", video_stats_.rtx_ssrc);
    video_json.AddNumber("packets_sent", video_stats_.packets_sent.load());
    video_json.AddNumber("bytes_sent", video_stats_.bytes_sent.load());
    video_json.AddNumber("rtx_packets_sent", video_stats_.rtx_packets_sent.load());
    video_json.AddNumber("rtx_bytes_sent", video_stats_.rtx_bytes_sent.load());
    video_json.AddNumber("packets_lost", video_stats_.packets_lost.load());
    video_json.AddNumber("packet_loss_rate", video_stats_.packet_loss_rate);
    video_json.AddNumber("bitrate", video_stats_.bitrate);
    video_json.AddNumber("frames_sent", video_stats_.frames_sent.load());
    video_json.AddNumber("key_frames_sent", video_stats_.key_frames_sent.load());
    video_json.AddNumber("frames_encoded", video_stats_.frames_encoded.load());
    video_json.AddNumber("width", video_stats_.width);
    video_json.AddNumber("height", video_stats_.height);
    video_json.AddNumber("fps", video_stats_.fps);
    video_json.AddString("codec", video_stats_.codec);
    video_json.AddNumber("nack_count", video_stats_.nack_count.load());
    video_json.AddNumber("pli_count", video_stats_.pli_count.load());
    video_json.AddNumber("fir_count", video_stats_.fir_count.load());
    
    // 构建音频统计
    JsonBuilder audio_json;
    audio_json.AddNumber("ssrc", audio_stats_.ssrc);
    audio_json.AddNumber("packets_sent", audio_stats_.packets_sent.load());
    audio_json.AddNumber("bytes_sent", audio_stats_.bytes_sent.load());
    audio_json.AddNumber("packets_lost", audio_stats_.packets_lost.load());
    audio_json.AddNumber("packet_loss_rate", audio_stats_.packet_loss_rate);
    audio_json.AddNumber("bitrate", audio_stats_.bitrate);
    audio_json.AddString("codec", audio_stats_.codec);
    
    // 构建RTP出站统计
    JsonBuilder rtp_outbound;
    rtp_outbound.AddObject("video", video_json.Build());
    rtp_outbound.AddObject("audio", audio_json.Build());
    
    // 构建RTCP统计
    JsonBuilder rtcp_json;
    rtcp_json.AddNumber("sr_count", rtcp_stats_.sr_count.load());
    rtcp_json.AddNumber("rr_count", rtcp_stats_.rr_count.load());
    rtcp_json.AddNumber("rtt", rtcp_stats_.rtt);
    rtcp_json.AddNumber("jitter", rtcp_stats_.jitter);
    
    // 构建质量评估
    JsonBuilder quality_json;
    quality_json.AddNumber("mos_score", quality_metrics_.mos_score);
    quality_json.AddString("quality_level", quality_metrics_.quality_level);
    quality_json.AddNumber("video_quality_score", quality_metrics_.video_quality_score);
    quality_json.AddNumber("audio_quality_score", quality_metrics_.audio_quality_score);
    
    // 构建完整的JSON
    JsonBuilder root;
    root.AddObject("basic_info", basic_info.Build());
    root.AddObject("rtp_outbound", rtp_outbound.Build());
    root.AddObject("rtcp", rtcp_json.Build());
    root.AddObject("quality", quality_json.Build());
    
    return root.Build();
}

} // namespace gb_media_server
