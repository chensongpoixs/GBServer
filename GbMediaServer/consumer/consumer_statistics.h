/***********************************************************************************************
created: 		2025-10-26

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
    
    // 获取会话名称
    const std::string& GetSessionName() const { return session_name_; }
    
    // 获取统计数据的getter方法（Getter methods for statistics data）
    // @author chensong
    // @date 2025-10-18
    uint64_t GetVideoBytesSent() const { return video_stats_.bytes_sent.load(); }
    uint64_t GetAudioBytesSent() const { return audio_stats_.bytes_sent.load(); }
    uint64_t GetVideoPacketsSent() const { return video_stats_.packets_sent.load(); }
    uint64_t GetAudioPacketsSent() const { return audio_stats_.packets_sent.load(); }
    uint32_t GetVideoFramesSent() const { return video_stats_.frames_sent.load(); }
    uint32_t GetNackCount() const { return video_stats_.nack_count.load(); }
    uint32_t GetPliCount() const { return video_stats_.pli_count.load(); }
    double GetVideoBitrate() const { return video_stats_.bitrate; }
    double GetAudioBitrate() const { return audio_stats_.bitrate; }
    double GetVideoPacketLossRate() const { return video_stats_.packet_loss_rate; }
    double GetRtt() const { return rtcp_stats_.rtt; }
    
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
