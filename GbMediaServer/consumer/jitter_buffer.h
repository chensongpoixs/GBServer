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


#ifndef _C_GB_MEDIA_SERVER_JITTER_BUFFER_H_
#define _C_GB_MEDIA_SERVER_JITTER_BUFFER_H_

#include <deque>
#include <map>
#include <mutex>
#include <cstdint>
#include <algorithm>
#include <memory>
#include "libmedia_codec/encoded_image.h"

namespace gb_media_server {

struct JitterBufferPacket {
    libmedia_codec::EncodedImage frame;
    int64_t timestamp_ms;       // 帧到达时间戳
    uint32_t rtp_timestamp;     // RTP时间戳
    uint16_t seq;               // RTP序列号
    bool is_key_frame;          // 是否是关键帧
};

class JitterBuffer {
public:
    JitterBuffer() 
        : target_delay_ms_(100)
        , min_delay_ms_(30)
        , max_delay_ms_(200)
        , jitter_ms_(0)
        , rtt_ms_(0)
        , last_update_time_ms_(0) {
    }

    void SetTargetDelay(int delay_ms) {
        target_delay_ms_ = std::max(min_delay_ms_, std::min(max_delay_ms_, delay_ms));
    }

    void SetDelayRange(int min_ms, int max_ms) {
        min_delay_ms_ = min_ms;
        max_delay_ms_ = max_ms;
        if (target_delay_ms_ < min_delay_ms_) {
            target_delay_ms_ = min_delay_ms_;
        }
        if (target_delay_ms_ > max_delay_ms_) {
            target_delay_ms_ = max_delay_ms_;
        }
    }

    void UpdateNetworkStats(double jitter, double rtt) {
        jitter_ms_ = jitter;
        rtt_ms_ = rtt;
        last_update_time_ms_ = rtc::SystemTimeMillis();
        
        int calculated = static_cast<int>(2 * jitter + rtt / 2 + 20);
        int new_target = std::max(min_delay_ms_, std::min(max_delay_ms_, calculated));
        
        if (new_target > target_delay_ms_) {
            target_delay_ms_ = (target_delay_ms_ + new_target) / 2;
        } else {
            target_delay_ms_ = new_target;
        }
    }

    void AddFrame(const libmedia_codec::EncodedImage& frame, uint32_t rtp_timestamp, uint16_t seq, bool is_key_frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        int64_t now_ms = rtc::SystemTimeMillis();
        
        JitterBufferPacket packet;
        packet.frame = frame;
        packet.timestamp_ms = now_ms;
        packet.rtp_timestamp = rtp_timestamp;
        packet.seq = seq;
        packet.is_key_frame = is_key_frame;
        
        packet_buffer_.push_back(packet);
        
        if (is_key_frame) {
            auto it = packet_buffer_.begin();
            while (it != packet_buffer_.end()) {
                if (!it->is_key_frame && it->seq < seq) {
                    it = packet_buffer_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        while (packet_buffer_.size() > 100) {
            packet_buffer_.pop_front();
        }
    }

    bool GetFrame(const JitterBufferPacket** out_packet, int64_t now_ms) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (packet_buffer_.empty()) {
            *out_packet = nullptr;
            return false;
        }
        
        JitterBufferPacket& front = packet_buffer_.front();
        int64_t playout_time = front.timestamp_ms + target_delay_ms_;
        
        if (now_ms >= playout_time) {
            *out_packet = &front;
            return true;
        }
        
        *out_packet = nullptr;
        return false;
    }

    void PopFrame() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!packet_buffer_.empty()) {
            packet_buffer_.pop_front();
        }
    }

    int GetTargetDelay() const {
        return target_delay_ms_;
    }

    int GetBufferSize() {
        std::lock_guard<std::mutex> lock(mutex_);
        return static_cast<int>(packet_buffer_.size());
    }

    bool HasFrames() {
        std::lock_guard<std::mutex> lock(mutex_);
        return !packet_buffer_.empty();
    }

    int64_t GetNextFramePlayTime() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (packet_buffer_.empty()) {
            return 0;
        }
        const JitterBufferPacket& front = packet_buffer_.front();
        return front.timestamp_ms + target_delay_ms_;
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        packet_buffer_.clear();
    }

private:
    std::deque<JitterBufferPacket> packet_buffer_;
    mutable std::mutex mutex_;
    
    int target_delay_ms_;
    int min_delay_ms_;
    int max_delay_ms_;
    double jitter_ms_;
    double rtt_ms_;
    int64_t last_update_time_ms_;
};

} // namespace gb_media_server

#endif // _C_GB_MEDIA_SERVER_JITTER_BUFFER_H_
