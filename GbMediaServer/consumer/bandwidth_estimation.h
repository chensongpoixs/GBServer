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

#ifndef _C_GB_MEDIA_SERVER_BANDWIDTH_ESTIMATION_H_
#define _C_GB_MEDIA_SERVER_BANDWIDTH_ESTIMATION_H_

#include <stdint.h>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace gb_media_server {

inline int64_t GetSystemTimeMs() {
#if defined(_WIN32)
    return GetTickCount64();
#else
    return (int64_t)(clock() * 1000 / CLOCKS_PER_SEC);
#endif
}

class BandwidthEstimation {
public:
    BandwidthEstimation()
        : min_bitrate_bps_(100000)
        , max_bitrate_bps_(10000000)
        , target_bitrate_bps_(500000)
        , last_update_time_ms_(0)
        , packets_lost_(0)
        , packets_received_(0)
        , last_fraction_loss_(0) {
    }

    void SetBitrates(int32_t min_bps, int32_t max_bps, int32_t initial_bps) {
        min_bitrate_bps_ = min_bps;
        max_bitrate_bps_ = max_bps;
        target_bitrate_bps_ = std::max(min_bps, std::min(max_bps, initial_bps));
    }

    void UpdatePacketsLost(int64_t lost, int64_t expected) {
        packets_lost_ = lost;
        packets_received_ = expected;
        
        if (expected > 0) {
            int64_t fraction = (lost * 100) / expected;
            last_fraction_loss_ = static_cast<int>(fraction);
            
            int64_t now_ms = GetSystemTimeMs();
            if (now_ms - last_update_time_ms_ > 1000) {
                UpdateBitrate(fraction);
                last_update_time_ms_ = now_ms;
            }
        }
    }

    void UpdateRtt(int32_t rtt_ms) {
        rtt_ms_ = rtt_ms;
    }

    int32_t GetTargetBitrate() const {
        return target_bitrate_bps_;
    }

    int32_t GetRtt() const {
        return rtt_ms_;
    }

    int GetFractionLoss() const {
        return last_fraction_loss_;
    }

private:
    void UpdateBitrate(int64_t loss_fraction) {
        if (loss_fraction > 20) {
            target_bitrate_bps_ = static_cast<int32_t>(target_bitrate_bps_ * 0.7);
        } else if (loss_fraction > 10) {
            target_bitrate_bps_ = static_cast<int32_t>(target_bitrate_bps_ * 0.85);
        } else if (loss_fraction < 2 && rtt_ms_ < 100) {
            target_bitrate_bps_ = static_cast<int32_t>(target_bitrate_bps_ * 1.1);
        }
        
        target_bitrate_bps_ = std::max(min_bitrate_bps_, 
            std::min(max_bitrate_bps_, target_bitrate_bps_));
    }

    int32_t min_bitrate_bps_;
    int32_t max_bitrate_bps_;
    int32_t target_bitrate_bps_;
    int32_t rtt_ms_;
    int64_t last_update_time_ms_;
    int64_t packets_lost_;
    int64_t packets_received_;
    int last_fraction_loss_;
};

} // namespace gb_media_server

#endif // _C_GB_MEDIA_SERVER_BANDWIDTH_ESTIMATION_H_
