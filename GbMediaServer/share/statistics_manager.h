/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#ifndef _GB_MEDIA_SERVER_STATISTICS_MANAGER_H_
#define _GB_MEDIA_SERVER_STATISTICS_MANAGER_H_

#include <string>
#include <memory>
#include <map>
#include <mutex>
#include "producer/producer_statistics.h"
#include "consumer/consumer_statistics.h"
#include <vector>
namespace gb_media_server {

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 统计管理器（Statistics Manager）
 * 
 * 单例模式，管理所有会话的统计数据。
 * 
 * 主要功能：
 * 1. 注册和注销Producer/Consumer统计
 * 2. 查询统计数据
 * 3. 生成会话级统计
 * 4. 生成系统级统计
 */
class StatisticsManager {
public:
    /**
     * @brief 获取单例实例
     */
    static StatisticsManager& GetInstance();
    
    /**
     * @brief 注册Producer统计
     * 
     * @param session_name 会话名称
     * @param statistics 统计对象
     */
    void RegisterProducer(const std::string& session_name,
                         std::shared_ptr<ProducerStatistics> statistics);
    
    /**
     * @brief 注销Producer统计
     * 
     * @param session_name 会话名称
     */
    void UnregisterProducer(const std::string& session_name);
    
    /**
     * @brief 注册Consumer统计
     * 
     * @param consumer_id Consumer ID
     * @param statistics 统计对象
     */
    void RegisterConsumer(const std::string& consumer_id,
                         std::shared_ptr<ConsumerStatistics> statistics);
    
    /**
     * @brief 注销Consumer统计
     * 
     * @param consumer_id Consumer ID
     */
    void UnregisterConsumer(const std::string& consumer_id);
    
    /**
     * @brief 获取Producer统计
     * 
     * @param session_name 会话名称
     * @return Producer统计对象，如果不存在返回nullptr
     */
    std::shared_ptr<ProducerStatistics> GetProducerStatistics(const std::string& session_name);
    
    /**
     * @brief 获取Consumer统计
     * 
     * @param consumer_id Consumer ID
     * @return Consumer统计对象，如果不存在返回nullptr
     */
    std::shared_ptr<ConsumerStatistics> GetConsumerStatistics(const std::string& consumer_id);
    
    /**
     * @brief 获取会话的所有Consumer统计
     * 
     * @param session_name 会话名称
     * @return Consumer统计对象列表
     */
    std::vector<std::shared_ptr<ConsumerStatistics>> GetSessionConsumers(const std::string& session_name);
    
    /**
     * @brief 获取会话统计JSON
     * 
     * @param session_name 会话名称
     * @return JSON格式的会话统计
     */
    std::string GetSessionStatsJson(const std::string& session_name);
    
    /**
     * @brief 获取所有会话统计JSON
     * 
     * @return JSON格式的所有会话统计
     */
    std::string GetAllSessionsStatsJson();
    
    /**
     * @brief 获取系统统计JSON
     * 
     * @return JSON格式的系统统计
     */
    std::string GetSystemStatsJson();
    
    /**
     * @brief 启动统计更新定时器
     * 
     * @author chensong
     * @date 2025-10-18
     */
    void StartUpdateTimer();
    
    /**
     * @brief 停止统计更新定时器
     * 
     * @author chensong
     * @date 2025-10-18
     */
    void StopUpdateTimer();
    
private:
    StatisticsManager();
    ~StatisticsManager();
    
    // 禁止拷贝和赋值
    StatisticsManager(const StatisticsManager&) = delete;
    StatisticsManager& operator=(const StatisticsManager&) = delete;
    
    /**
     * @brief 更新所有统计对象
     * 
     * @author chensong
     * @date 2025-10-18
     */
    void UpdateAllStatistics();
    
    /**
     * @brief 定时器回调
     * 
     * @author chensong
     * @date 2025-10-18
     */
    void OnUpdateTimer();
    
private:
    std::mutex mutex_;
    
    // session_name -> ProducerStatistics
    std::map<std::string, std::shared_ptr<ProducerStatistics>> producer_stats_;
    
    // consumer_id -> ConsumerStatistics
    std::map<std::string, std::shared_ptr<ConsumerStatistics>> consumer_stats_;
    
    // session_name -> consumer_ids
    std::map<std::string, std::vector<std::string>> session_consumers_;
    
    // 定时器运行标志（Timer running flag）
    // @author chensong
    // @date 2025-10-18
    bool timer_running_ = false;
};

} // namespace gb_media_server

#endif // _GB_MEDIA_SERVER_STATISTICS_MANAGER_H_
