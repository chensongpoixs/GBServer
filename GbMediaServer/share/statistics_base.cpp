/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/

#include "share/statistics_base.h"

namespace gb_media_server {

StatisticsBase::StatisticsBase() {
    created_time_ = GetCurrentTimeMs();
    last_update_time_ = created_time_;
}

StatisticsBase::~StatisticsBase() {
}

void StatisticsBase::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    created_time_ = GetCurrentTimeMs();
    last_update_time_ = created_time_;
}

void StatisticsBase::UpdateTimestamp() {
    last_update_time_ = GetCurrentTimeMs();
}

int64_t StatisticsBase::GetDuration() const {
    int64_t current_time = GetCurrentTimeMs();
    return (current_time - created_time_) / 1000;  // 转换为秒
}

int64_t StatisticsBase::GetCurrentTimeMs() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

} // namespace gb_media_server
