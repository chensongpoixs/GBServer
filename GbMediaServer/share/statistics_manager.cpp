/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/

#include "share/statistics_manager.h"
#include "utils/json_utils.h"
#include <algorithm>

namespace gb_media_server {

StatisticsManager& StatisticsManager::GetInstance() {
    static StatisticsManager instance;
    return instance;
}

StatisticsManager::StatisticsManager() {
}

StatisticsManager::~StatisticsManager() {
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
    const std::string& session_name = statistics->GetConsumerId();
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
    if (producer_it != producer_stats_.end()) {
        // 注意：这里需要Producer统计类提供getter方法
        // 暂时通过解析JSON获取（不是最优方案，后续可以添加getter方法）
    }
    
    // 从所有Consumer收集数据
    if (session_consumers_it != session_consumers_.end()) {
        int valid_consumer_count = 0;
        for (const auto& consumer_id : session_consumers_it->second) {
            auto consumer_it = consumer_stats_.find(consumer_id);
            if (consumer_it != consumer_stats_.end()) {
                valid_consumer_count++;
                // 同样需要getter方法，暂时简化处理
            }
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
    JsonBuilder aggregated;
    
    uint64_t total_bytes_received = 0;
    uint64_t total_bytes_sent = 0;
    uint64_t total_packets_received = 0;
    uint64_t total_packets_sent = 0;
    int64_t active_sessions = 0;
    int64_t waiting_sessions = 0;
    
    // 遍历所有会话统计
    for (const auto& pair : producer_stats_) {
        const std::string& session_name = pair.first;
        
        // 统计会话状态
        auto it = session_consumers_.find(session_name);
        if (it != session_consumers_.end() && !it->second.empty()) {
            active_sessions++;
        } else {
            waiting_sessions++;
        }
        
        // 这里可以累加更多统计数据
        // 需要Producer和Consumer提供getter方法
    }
    
    aggregated.AddNumber("total_bytes_received", total_bytes_received);
    aggregated.AddNumber("total_bytes_sent", total_bytes_sent);
    aggregated.AddNumber("total_packets_received", total_packets_received);
    aggregated.AddNumber("total_packets_sent", total_packets_sent);
    aggregated.AddNumber("active_sessions", (int64_t)active_sessions);
    aggregated.AddNumber("waiting_sessions", (int64_t)waiting_sessions);
    
    root.AddObject("aggregated", aggregated.Build());
    
    // 会话列表摘要
    JsonArrayBuilder sessions_summary;
    for (const auto& pair : producer_stats_) {
        const std::string& session_name = pair.first;
        
        JsonBuilder session;
        session.AddString("session_name", session_name);
        
        int64_t consumer_count = 0;
        auto it = session_consumers_.find(session_name);
        if (it != session_consumers_.end()) {
            consumer_count = it->second.size();
        }
        
        session.AddNumber("consumer_count",  consumer_count);
        session.AddNumber("duration_ms", pair.second->GetDuration());
        
        sessions_summary.AddObject(session.Build());
    }
    root.AddObject("sessions", sessions_summary.Build());
    
    return root.Build();
}

} // namespace gb_media_server
