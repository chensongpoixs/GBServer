/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/

#include "share/statistics_manager.h"
#include "utils/json_utils.h"
#include "server/gb_media_service.h"
#include "rtc_base/task_utils/to_queued_task.h"
#include <algorithm>
#include "consumer/consumer_statistics.h"
#include "share/statistics_base.h"


namespace gb_media_server {

StatisticsManager& StatisticsManager::GetInstance() {
    static StatisticsManager instance;
    return instance;
}

StatisticsManager::StatisticsManager() {
    // 启动统计更新定时器（Start statistics update timer）
    // @author chensong
    // @date 2025-10-18
    StartUpdateTimer();
}

StatisticsManager::~StatisticsManager() {
    // 停止统计更新定时器（Stop statistics update timer）
    // @author chensong
    // @date 2025-10-18
    StopUpdateTimer();
}

void StatisticsManager::RegisterProducer(const std::string& session_name,
                                        std::shared_ptr<ProducerStatistics> statistics) {
    std::lock_guard<std::mutex> lock(mutex_);
    producer_stats_[session_name] = statistics;
}

void StatisticsManager::UnregisterProducer(const std::string& session_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    producer_stats_.erase(session_name);
}

void StatisticsManager::RegisterConsumer(const std::string& consumer_id,
                                        std::shared_ptr<ConsumerStatistics> statistics) {
    std::lock_guard<std::mutex> lock(mutex_);
    consumer_stats_[consumer_id] = statistics;
    
    // 添加到会话的Consumer列表
    const std::string& session_name = statistics->GetSessionName();
    session_consumers_[session_name].push_back(consumer_id);
}

void StatisticsManager::UnregisterConsumer(const std::string& consumer_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 从会话的Consumer列表中移除
    for (auto& pair : session_consumers_) {
        auto& consumers = pair.second;
        consumers.erase(std::remove(consumers.begin(), consumers.end(), consumer_id), 
                       consumers.end());
    }
    
    consumer_stats_.erase(consumer_id);
}

std::shared_ptr<ProducerStatistics> StatisticsManager::GetProducerStatistics(
    const std::string& session_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = producer_stats_.find(session_name);
    if (it != producer_stats_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<ConsumerStatistics> StatisticsManager::GetConsumerStatistics(
    const std::string& consumer_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = consumer_stats_.find(consumer_id);
    if (it != consumer_stats_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<ConsumerStatistics>> StatisticsManager::GetSessionConsumers(
    const std::string& session_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<ConsumerStatistics>> result;
    
    auto it = session_consumers_.find(session_name);
    if (it != session_consumers_.end()) {
        for (const auto& consumer_id : it->second) {
            auto consumer_it = consumer_stats_.find(consumer_id);
            if (consumer_it != consumer_stats_.end()) {
                result.push_back(consumer_it->second);
            }
        }
    }
    
    return result;
}

std::string StatisticsManager::GetSessionStatsJson(const std::string& session_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    JsonBuilder root;
    root.AddString("session_name", session_name);
    root.AddNumber("timestamp", StatisticsBase::GetCurrentTimeMs());
    
    // 添加Producer统计
    auto producer_it = producer_stats_.find(session_name);
    if (producer_it != producer_stats_.end()) {
        root.AddObject("producer", producer_it->second->ToJson());
    } else {
        root.AddObject("producer", "null");
    }
    
    // 添加Consumer统计数组
    JsonArrayBuilder consumers_array;
    auto session_it = session_consumers_.find(session_name);
    if (session_it != session_consumers_.end()) {
        for (const auto& consumer_id : session_it->second) {
            auto consumer_it = consumer_stats_.find(consumer_id);
            if (consumer_it != consumer_stats_.end()) {
                consumers_array.AddObject(consumer_it->second->ToJson());
            }
        }
    }
    root.AddObject("consumers", consumers_array.Build());
    
    // 计算汇总信息（Summary）
    JsonBuilder summary;
    
    // 基本计数
    int producer_count = (producer_it != producer_stats_.end()) ? 1 : 0;
    int consumer_count = 0;
    auto session_consumers_it = session_consumers_.find(session_name);
    if (session_consumers_it != session_consumers_.end()) {
        consumer_count = session_consumers_it->second.size();
    }
    
    summary.AddNumber("producer_count",(int64_t) producer_count);
    summary.AddNumber("consumer_count", (int64_t)consumer_count);
    
    // 聚合统计数据
    uint64_t total_bytes_received = 0;
    uint64_t total_packets_received = 0;
    uint64_t total_bytes_sent = 0;
    uint64_t total_packets_sent = 0;
    uint64_t total_frames_received = 0;
    uint64_t total_frames_sent = 0;
    uint32_t total_nack_count = 0;
    uint32_t total_pli_count = 0;
    double avg_bitrate_in = 0.0;
    double avg_bitrate_out = 0.0;
    double max_rtt = 0.0;
    double avg_packet_loss_rate = 0.0;
    
    // 从Producer收集数据
    // @author chensong
    // @date 2025-10-18
    if (producer_it != producer_stats_.end()) {
        auto producer = producer_it->second;
        total_bytes_received = producer->GetVideoBytesReceived() + producer->GetAudioBytesReceived();
        total_packets_received = producer->GetVideoPacketsReceived() + producer->GetAudioPacketsReceived();
        total_frames_received = producer->GetVideoFramesReceived();
        total_nack_count = producer->GetNackCount();
        total_pli_count = producer->GetPliCount();
        avg_bitrate_in = producer->GetVideoBitrate() + producer->GetAudioBitrate();
        max_rtt = producer->GetRtt();
        avg_packet_loss_rate = producer->GetVideoPacketLossRate();
    }
    
    // 从所有Consumer收集数据
    // @author chensong
    // @date 2025-10-18
    if (session_consumers_it != session_consumers_.end()) {
        int valid_consumer_count = 0;
        double total_bitrate_out = 0.0;
        double total_packet_loss = 0.0;
        double total_rtt = 0.0;
        
        for (const auto& consumer_id : session_consumers_it->second) {
            auto consumer_it = consumer_stats_.find(consumer_id);
            if (consumer_it != consumer_stats_.end()) {
                auto consumer = consumer_it->second;
                total_bytes_sent += consumer->GetVideoBytesSent() + consumer->GetAudioBytesSent();
                total_packets_sent += consumer->GetVideoPacketsSent() + consumer->GetAudioPacketsSent();
                total_frames_sent += consumer->GetVideoFramesSent();
                total_bitrate_out += consumer->GetVideoBitrate() + consumer->GetAudioBitrate();
                total_packet_loss += consumer->GetVideoPacketLossRate();
                total_rtt += consumer->GetRtt();
                valid_consumer_count++;
            }
        }
        
        if (valid_consumer_count > 0) {
            avg_bitrate_out = total_bitrate_out / valid_consumer_count;
            avg_packet_loss_rate = (avg_packet_loss_rate + total_packet_loss / valid_consumer_count) / 2.0;
            max_rtt = std::max(max_rtt, total_rtt / valid_consumer_count);
        }
    }
    
    summary.AddNumber("total_bytes_received", total_bytes_received);
    summary.AddNumber("total_packets_received", total_packets_received);
    summary.AddNumber("total_bytes_sent", total_bytes_sent);
    summary.AddNumber("total_packets_sent", total_packets_sent);
    summary.AddNumber("total_frames_received", total_frames_received);
    summary.AddNumber("total_frames_sent", total_frames_sent);
    summary.AddNumber("total_nack_count", total_nack_count);
    summary.AddNumber("total_pli_count", total_pli_count);
    summary.AddNumber("avg_bitrate_in_bps", avg_bitrate_in);
    summary.AddNumber("avg_bitrate_out_bps", avg_bitrate_out);
    summary.AddNumber("max_rtt_ms", max_rtt);
    summary.AddNumber("avg_packet_loss_rate", avg_packet_loss_rate);
    
    // 会话状态
    std::string session_state = "unknown";
    if (producer_count > 0 && consumer_count > 0) {
        session_state = "active";
    } else if (producer_count > 0) {
        session_state = "waiting_consumers";
    } else if (consumer_count > 0) {
        session_state = "waiting_producer";
    } else {
        session_state = "idle";
    }
    summary.AddString("session_state", session_state);
    
    // 会话持续时间（从Producer创建时间计算）
    int64_t session_duration = 0;
    if (producer_it != producer_stats_.end()) {
        session_duration = producer_it->second->GetDuration();
    }
    summary.AddNumber("session_duration_ms", session_duration);
    
    root.AddObject("summary", summary.Build());
    
    return root.Build();
}

std::string StatisticsManager::GetAllSessionsStatsJson() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    JsonArrayBuilder sessions_array;
    
    // 遍历所有会话
    for (const auto& pair : producer_stats_) {
        const std::string& session_name = pair.first;
        
        JsonBuilder session_json;
        session_json.AddString("session_name", session_name);
        
        // 统计Consumer数量
        int consumer_count = 0;
        auto it = session_consumers_.find(session_name);
        if (it != session_consumers_.end()) {
            consumer_count = it->second.size();
        }
        
        session_json.AddNumber("producer_count", (int64_t)1);
        session_json.AddNumber("consumer_count", (int64_t)consumer_count);
        
        // 会话持续时间
        int64_t duration = pair.second->GetDuration();
        session_json.AddNumber("duration_ms", duration);
        
        // 会话状态
        std::string state = "unknown";
        if (consumer_count > 0) {
            state = "active";
        } else {
            state = "waiting_consumers";
        }
        session_json.AddString("state", state);
        
        // 创建时间
        session_json.AddNumber("created_at", pair.second->GetCreatedTime());
        
        sessions_array.AddObject(session_json.Build());
    }
    
    // 构建根对象
    JsonBuilder root;
    root.AddObject("sessions", sessions_array.Build());
    root.AddNumber("total_sessions", producer_stats_.size());
    root.AddNumber("total_producers", producer_stats_.size());
    root.AddNumber("total_consumers", consumer_stats_.size());
    root.AddNumber("timestamp", StatisticsBase::GetCurrentTimeMs());
    
    return root.Build();
}

std::string StatisticsManager::GetSystemStatsJson() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    JsonBuilder root;
    
    // 基本计数
    root.AddNumber("total_sessions", producer_stats_.size());
    root.AddNumber("total_producers", producer_stats_.size());
    root.AddNumber("total_consumers", consumer_stats_.size());
    root.AddNumber("timestamp", StatisticsBase::GetCurrentTimeMs());
    
    // 系统级聚合统计
    uint64_t total_bytes_received = 0;
    uint64_t total_bytes_sent = 0;
    uint64_t total_packets_received = 0;
    uint64_t total_packets_sent = 0;
    double total_bitrate_in = 0;
    double total_bitrate_out = 0;
    double total_packet_loss = 0;
    double total_rtt = 0;
    int64_t active_sessions = 0;
    int64_t waiting_sessions = 0;
    int valid_producer_count = 0;
    int valid_consumer_count = 0;
    
    // 遍历所有Producer统计
    // @author chensong
    // @date 2025-10-18
    for (const auto& pair : producer_stats_) {
        const std::string& session_name = pair.first;
        auto producer = pair.second;
        
        // 统计会话状态
        auto it = session_consumers_.find(session_name);
        if (it != session_consumers_.end() && !it->second.empty()) {
            active_sessions++;
        } else {
            waiting_sessions++;
        }
        
        // 累加Producer统计
        total_bytes_received += producer->GetVideoBytesReceived() + producer->GetAudioBytesReceived();
        total_packets_received += producer->GetVideoPacketsReceived() + producer->GetAudioPacketsReceived();
        total_bitrate_in += producer->GetVideoBitrate() + producer->GetAudioBitrate();
        total_packet_loss += producer->GetVideoPacketLossRate();
        total_rtt += producer->GetRtt();
        valid_producer_count++;
    }
    
    // 遍历所有Consumer统计
    // @author chensong
    // @date 2025-10-18
    for (const auto& pair : consumer_stats_) {
        auto consumer = pair.second;
        // 累加Consumer统计
        total_bytes_sent += consumer->GetVideoBytesSent() + consumer->GetAudioBytesSent();
        total_packets_sent += consumer->GetVideoPacketsSent() + consumer->GetAudioPacketsSent();
        total_bitrate_out += consumer->GetVideoBitrate() + consumer->GetAudioBitrate();
        valid_consumer_count++;
    }
    
    // 计算平均值
    double avg_bitrate_in = valid_producer_count > 0 ? total_bitrate_in / valid_producer_count : 0;
    double avg_bitrate_out = valid_consumer_count > 0 ? total_bitrate_out / valid_consumer_count : 0;
    double avg_packet_loss = valid_consumer_count > 0 ? total_packet_loss / valid_consumer_count : 0;
    double avg_rtt = valid_consumer_count > 0 ? total_rtt / valid_consumer_count : 0;
    
    // 添加聚合数据
    root.AddNumber("total_bytes_in", total_bytes_received);
    root.AddNumber("total_bytes_out", total_bytes_sent);
    root.AddNumber("total_packets_in", total_packets_received);
    root.AddNumber("total_packets_out", total_packets_sent);
    root.AddNumber("avg_bitrate_in", (int64_t)avg_bitrate_in);
    root.AddNumber("avg_bitrate_out", (int64_t)avg_bitrate_out);
    root.AddNumber("avg_packet_loss", avg_packet_loss);
    root.AddNumber("avg_rtt", avg_rtt);
    root.AddNumber("active_sessions", active_sessions);
    root.AddNumber("waiting_sessions", waiting_sessions);
    
    return root.Build();
}

//} // namespace gb_media_server

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 启动统计更新定时器（Start statistics update timer）
 * 
 * 该方法启动一个定时器，每秒调用一次UpdateAllStatistics()方法，
 * 更新所有Producer和Consumer的统计数据（码率、帧率等派生指标）。
 * 
 * 定时器机制：
 * - 使用PostDelayedTask异步调度
 * - 每次回调后重新调度下一次（递归调用）
 * - 间隔时间：1000毫秒（1秒）
 * 
 * @note 该方法在StatisticsManager构造函数中自动调用
 * @note 如果定时器已经运行，则不会重复启动
 */
void StatisticsManager::StartUpdateTimer() {
    if (timer_running_) {
        return;
    }
    
    timer_running_ = true;
    
    // 启动第一次定时器回调
    OnUpdateTimer();
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 停止统计更新定时器（Stop statistics update timer）
 * 
 * 该方法停止统计更新定时器，不再更新统计数据。
 * 
 * @note 该方法在StatisticsManager析构函数中自动调用
 * @note 由于使用PostDelayedTask，无法立即停止正在等待的任务
 */
void StatisticsManager::StopUpdateTimer() {
    timer_running_ = false;
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 定时器回调（Timer callback）
 * 
 * 该方法是定时器的回调函数，每秒调用一次。
 * 
 * 执行流程：
 * 1. 检查定时器是否仍在运行
 * 2. 调用UpdateAllStatistics()更新所有统计数据
 * 3. 重新调度下一次回调（1秒后）
 * 
 * @note 使用递归调度实现定时器循环
 * @note 使用lambda捕获this指针，确保对象生命周期
 */
void StatisticsManager::OnUpdateTimer() {
    if (!timer_running_) {
        return;
    }
    
    // 使用worker线程的PostDelayedTask调度定时任务
    // 参考RtcProducer的实现
    gb_media_server::GbMediaService::GetInstance().worker_thread()->PostDelayedTask(
        webrtc::ToQueuedTask([this]() {
            if (!timer_running_) {
                return;
            }
            
            // 更新所有统计数据
            UpdateAllStatistics();
            
            // 递归调度下一次回调
            OnUpdateTimer();
        }), 
        1000  // 1秒间隔
    );
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 更新所有统计对象（Update all statistics objects）
 * 
 * 该方法遍历所有注册的Producer和Consumer统计对象，
 * 调用它们的Update()方法，计算派生指标（码率、帧率、丢包率等）。
 * 
 * 更新内容：
 * - Producer统计：视频/音频码率、帧率、丢包率、抖动等
 * - Consumer统计：视频/音频码率、帧率、丢包率、RTT等
 * 
 * 线程安全：
 * - 使用互斥锁保护统计对象的访问
 * - 每个统计对象的Update()方法内部也是线程安全的
 * 
 * 错误处理：
 * - 使用try-catch捕获异常，避免单个对象的错误影响其他对象
 * 
 * @note 该方法每秒调用一次
 * @note 如果统计对象很多（>1000个），可能需要优化性能
 */
void StatisticsManager::UpdateAllStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 更新所有Producer统计
    for (auto& pair : producer_stats_) {
        try {
            if (pair.second) {
                pair.second->Update();
            }
        } catch (const std::exception& e) {
            // 记录错误但继续处理其他对象
            // LOG_ERROR("更新Producer统计失败: " << e.what());
        }
    }
    
    // 更新所有Consumer统计
    for (auto& pair : consumer_stats_) {
        try {
            if (pair.second) {
                pair.second->Update();
            }
        } catch (const std::exception& e) {
            // 记录错误但继续处理其他对象
            // LOG_ERROR("更新Consumer统计失败: " << e.what());
        }
    }
}

} // namespace gb_media_server