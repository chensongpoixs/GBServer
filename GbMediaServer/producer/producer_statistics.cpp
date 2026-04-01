/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/

#include "producer/producer_statistics.h"
#include "utils/json_utils.h"
#include <sstream>

namespace gb_media_server {

ProducerStatistics::ProducerStatistics(const std::string& session_name,
                                       const std::string& stream_name,
                                       const std::string& producer_type)
    : StatisticsBase()
    , session_name_(session_name)
    , stream_name_(stream_name)
    , producer_type_(producer_type)
    , state_("connecting")
    , last_bitrate_calc_time_(0)
    , last_video_bytes_(0)
    , last_audio_bytes_(0)
    , last_fps_calc_time_(0)
    , last_video_frames_(0)
{
}

ProducerStatistics::~ProducerStatistics() {
}

void ProducerStatistics::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    StatisticsBase::Reset();
    
    // 重置视频统计
    video_stats_.packets_received = 0;
    video_stats_.bytes_received = 0;
    video_stats_.packets_lost = 0;
    video_stats_.frames_received = 0;
    video_stats_.key_frames_received = 0;
    video_stats_.frames_decoded = 0;
    video_stats_.frames_dropped = 0;
    video_stats_.jitter = 0.0;
    video_stats_.bitrate = 0.0;
    video_stats_.fps = 0.0;
    video_stats_.packet_loss_rate = 0.0;
    
    // 重置音频统计
    audio_stats_.packets_received = 0;
    audio_stats_.bytes_received = 0;
    audio_stats_.packets_lost = 0;
    audio_stats_.jitter = 0.0;
    audio_stats_.bitrate = 0.0;
    audio_stats_.packet_loss_rate = 0.0;
    
    // 重置RTCP统计
    rtcp_stats_.sr_count = 0;
    rtcp_stats_.rr_count = 0;
    rtcp_stats_.nack_count = 0;
    rtcp_stats_.pli_count = 0;
    rtcp_stats_.fir_count = 0;
    rtcp_stats_.rtt = 0.0;
    
    // 重置临时变量
    last_bitrate_calc_time_ = 0;
    last_video_bytes_ = 0;
    last_audio_bytes_ = 0;
    last_fps_calc_time_ = 0;
    last_video_frames_ = 0;
}

void ProducerStatistics::OnRtpPacketReceived(bool is_video, size_t packet_size) {
    if (is_video) {
        video_stats_.packets_received++;
        video_stats_.bytes_received += packet_size;
    } else {
        audio_stats_.packets_received++;
        audio_stats_.bytes_received += packet_size;
    }
    UpdateTimestamp();
}

void ProducerStatistics::OnRtpPacketLost(bool is_video, uint32_t lost_count) {
    if (is_video) {
        video_stats_.packets_lost += lost_count;
    } else {
        audio_stats_.packets_lost += lost_count;
    }
}

void ProducerStatistics::OnFrameReceived(bool is_video, bool is_key_frame) {
    if (is_video) {
        video_stats_.frames_received++;
        if (is_key_frame) {
            video_stats_.key_frames_received++;
        }
    }
    UpdateTimestamp();
}

void ProducerStatistics::OnFrameDecoded(bool is_video) {
    if (is_video) {
        video_stats_.frames_decoded++;
    }
}

void ProducerStatistics::OnFrameDropped(bool is_video) {
    if (is_video) {
        video_stats_.frames_dropped++;
    }
}

void ProducerStatistics::OnRtcpSRReceived() {
    rtcp_stats_.sr_count++;
}

void ProducerStatistics::OnRtcpRRSent() {
    rtcp_stats_.rr_count++;
}

void ProducerStatistics::OnNackReceived(uint32_t count) {
    rtcp_stats_.nack_count += count;
}

void ProducerStatistics::OnPliReceived() {
    rtcp_stats_.pli_count++;
}

void ProducerStatistics::OnFirReceived() {
    rtcp_stats_.fir_count++;
}

void ProducerStatistics::UpdateRtt(double rtt_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    rtcp_stats_.rtt = rtt_ms;
}

void ProducerStatistics::UpdateJitter(bool is_video, double jitter_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_video) {
        video_stats_.jitter = jitter_ms;
    } else {
        audio_stats_.jitter = jitter_ms;
    }
}

void ProducerStatistics::OnDtlsHandshakeStart() {
    std::lock_guard<std::mutex> lock(mutex_);
    security_stats_.state = "connecting";
    security_stats_.handshake_start_time = GetCurrentTimeMs();
}

void ProducerStatistics::OnDtlsHandshakeComplete(const std::string& dtls_cipher, 
                                                 const std::string& srtp_cipher) {
    std::lock_guard<std::mutex> lock(mutex_);
    security_stats_.state = "connected";
    security_stats_.dtls_cipher = dtls_cipher;
    security_stats_.srtp_cipher = srtp_cipher;
    if (security_stats_.handshake_start_time > 0) {
        security_stats_.handshake_duration = GetCurrentTimeMs() - security_stats_.handshake_start_time;
    }
    state_ = "connected";
}

void ProducerStatistics::OnDtlsHandshakeFailed() {
    std::lock_guard<std::mutex> lock(mutex_);
    security_stats_.state = "failed";
    state_ = "failed";
}

void ProducerStatistics::SetVideoInfo(uint32_t ssrc, uint32_t width, uint32_t height, 
                                     const std::string& codec) {
    std::lock_guard<std::mutex> lock(mutex_);
    video_stats_.ssrc = ssrc;
    video_stats_.width = width;
    video_stats_.height = height;
    video_stats_.codec = codec;
}

void ProducerStatistics::SetAudioInfo(uint32_t ssrc, uint32_t sample_rate, 
                                     uint32_t channels, const std::string& codec) {
    std::lock_guard<std::mutex> lock(mutex_);
    audio_stats_.ssrc = ssrc;
    audio_stats_.sample_rate = sample_rate;
    audio_stats_.channels = channels;
    audio_stats_.codec = codec;
}

void ProducerStatistics::SetState(const std::string& state) {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = state;
}

void ProducerStatistics::Update() {
    CalculateBitrate();
    CalculatePacketLossRate();
    CalculateFps();
}

void ProducerStatistics::CalculateBitrate() {
    int64_t current_time = GetCurrentTimeMs();
    
    if (last_bitrate_calc_time_ == 0) {
        last_bitrate_calc_time_ = current_time;
        last_video_bytes_ = video_stats_.bytes_received.load();
        last_audio_bytes_ = audio_stats_.bytes_received.load();
        return;
    }
    
    int64_t time_diff = current_time - last_bitrate_calc_time_;
    if (time_diff < 1000) {  // 至少1秒才计算一次
        return;
    }
    
    uint64_t current_video_bytes = video_stats_.bytes_received.load();
    uint64_t current_audio_bytes = audio_stats_.bytes_received.load();
    
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

void ProducerStatistics::CalculatePacketLossRate() {
    // 计算视频丢包率
    uint64_t video_total = video_stats_.packets_received.load() + video_stats_.packets_lost.load();
    if (video_total > 0) {
        video_stats_.packet_loss_rate = (video_stats_.packets_lost.load() * 100.0) / video_total;
    }
    
    // 计算音频丢包率
    uint64_t audio_total = audio_stats_.packets_received.load() + audio_stats_.packets_lost.load();
    if (audio_total > 0) {
        audio_stats_.packet_loss_rate = (audio_stats_.packets_lost.load() * 100.0) / audio_total;
    }
}

void ProducerStatistics::CalculateFps() {
    int64_t current_time = GetCurrentTimeMs();
    
    if (last_fps_calc_time_ == 0) {
        last_fps_calc_time_ = current_time;
        last_video_frames_ = video_stats_.frames_received.load();
        return;
    }
    
    int64_t time_diff = current_time - last_fps_calc_time_;
    if (time_diff < 1000) {  // 至少1秒才计算一次
        return;
    }
    
    uint32_t current_frames = video_stats_.frames_received.load();
    uint32_t frames_diff = current_frames - last_video_frames_;
    
    // 计算帧率
    video_stats_.fps = (frames_diff * 1000.0) / time_diff;
    
    last_fps_calc_time_ = current_time;
    last_video_frames_ = current_frames;
}

std::string ProducerStatistics::ToJson() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 构建基础信息
    JsonBuilder basic_info;
    basic_info.AddString("session_name", session_name_);
    basic_info.AddString("stream_name", stream_name_);
    basic_info.AddString("producer_type", producer_type_);
    basic_info.AddNumber("created_time", created_time_);
    basic_info.AddNumber("duration", GetDuration());
    basic_info.AddString("state", state_);
    
    // 构建视频统计
    JsonBuilder video_json;
    video_json.AddNumber("ssrc", video_stats_.ssrc);
    video_json.AddNumber("packets_received", video_stats_.packets_received.load());
    video_json.AddNumber("bytes_received", video_stats_.bytes_received.load());
    video_json.AddNumber("packets_lost", video_stats_.packets_lost.load());
    video_json.AddNumber("packet_loss_rate", video_stats_.packet_loss_rate);
    video_json.AddNumber("jitter", video_stats_.jitter);
    video_json.AddNumber("bitrate", video_stats_.bitrate);
    video_json.AddNumber("frames_received", video_stats_.frames_received.load());
    video_json.AddNumber("key_frames_received", video_stats_.key_frames_received.load());
    video_json.AddNumber("frames_decoded", video_stats_.frames_decoded.load());
    video_json.AddNumber("frames_dropped", video_stats_.frames_dropped.load());
    video_json.AddNumber("width", video_stats_.width);
    video_json.AddNumber("height", video_stats_.height);
    video_json.AddNumber("fps", video_stats_.fps);
    video_json.AddString("codec", video_stats_.codec);
    
    // 构建音频统计
    JsonBuilder audio_json;
    audio_json.AddNumber("ssrc", audio_stats_.ssrc);
    audio_json.AddNumber("packets_received", audio_stats_.packets_received.load());
    audio_json.AddNumber("bytes_received", audio_stats_.bytes_received.load());
    audio_json.AddNumber("packets_lost", audio_stats_.packets_lost.load());
    audio_json.AddNumber("packet_loss_rate", audio_stats_.packet_loss_rate);
    audio_json.AddNumber("jitter", audio_stats_.jitter);
    audio_json.AddNumber("bitrate", audio_stats_.bitrate);
    audio_json.AddString("codec", audio_stats_.codec);
    audio_json.AddNumber("sample_rate", audio_stats_.sample_rate);
    audio_json.AddNumber("channels", audio_stats_.channels);
    
    // 构建RTP入站统计
    JsonBuilder rtp_inbound;
    rtp_inbound.AddObject("video", video_json.Build());
    rtp_inbound.AddObject("audio", audio_json.Build());
    
    // 构建RTCP统计
    JsonBuilder rtcp_json;
    rtcp_json.AddNumber("sr_count", rtcp_stats_.sr_count.load());
    rtcp_json.AddNumber("rr_count", rtcp_stats_.rr_count.load());
    rtcp_json.AddNumber("nack_count", rtcp_stats_.nack_count.load());
    rtcp_json.AddNumber("pli_count", rtcp_stats_.pli_count.load());
    rtcp_json.AddNumber("fir_count", rtcp_stats_.fir_count.load());
    rtcp_json.AddNumber("rtt", rtcp_stats_.rtt);
    
    // 构建安全统计
    JsonBuilder security_json;
    security_json.AddString("dtls_state", security_stats_.state);
    security_json.AddString("dtls_cipher", security_stats_.dtls_cipher);
    security_json.AddString("srtp_cipher", security_stats_.srtp_cipher);
    security_json.AddString("local_fingerprint", security_stats_.local_fingerprint);
    security_json.AddString("remote_fingerprint", security_stats_.remote_fingerprint);
    security_json.AddNumber("dtls_handshake_time", security_stats_.handshake_duration);
    
    // 构建完整的JSON
    JsonBuilder root;
    root.AddObject("basic_info", basic_info.Build());
    root.AddObject("rtp_inbound", rtp_inbound.Build());
    root.AddObject("rtcp", rtcp_json.Build());
    root.AddObject("security", security_json.Build());
    
    return root.Build();
}

} // namespace gb_media_server
