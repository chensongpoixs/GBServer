/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#ifndef _GB_MEDIA_SERVER_WS_STATS_SERVICE_H_
#define _GB_MEDIA_SERVER_WS_STATS_SERVICE_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>

// WebSocket功能开关
// 设置为0则禁用WebSocket功能（代码可以编译但不会实际工作）
// 设置为1则启用WebSocket功能（需要安装websocketpp和nlohmann/json库）
#define ENABLE_WEBSOCKET 1

#if ENABLE_WEBSOCKET
// 使用standalone ASIO，不依赖Boost
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_INTERNAL_

// WebSocket库
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// JSON库
#include <nlohmann/json.hpp>
#endif

namespace gb_media_server {

// 订阅目标类型
enum class SubscriptionTarget {
    SYSTEM,      // 系统统计
    SESSIONS,    // 会话列表
    SESSION,     // 单个会话
    PRODUCER,    // Producer统计
    CONSUMER     // Consumer统计
};

// 订阅信息
struct Subscription {
    SubscriptionTarget target;
    std::string id;  // session_name or consumer_id
    int64_t interval;  // 推送间隔（毫秒）
    int64_t last_push_time;  // 上次推送时间
};

// 连接信息（简化版本，不依赖websocketpp）
struct ConnectionInfo {
    std::string connection_id;
    std::vector<Subscription> subscriptions;
    int64_t last_ping_time;
    int64_t connected_at;
    int64_t sent_messages;
    int64_t sent_bytes;
    bool authenticated;
    void* user_data;  // 用于存储websocket连接句柄
};

/**
 * @author chensong
 * @date 2025-10-18
 * @brief WebSocket统计服务
 * 
 * 提供基于WebSocket的实时统计数据推送服务
 * 
 * 功能：
 * 1. 管理WebSocket连接
 * 2. 处理客户端订阅请求
 * 3. 定时推送统计数据
 * 4. 支持多客户端连接
 * 
 * 使用方法：
 * 1. 在配置文件中启用WebSocket服务
 * 2. 调用Start()启动服务
 * 3. 客户端连接后发送订阅消息
 * 4. 服务器自动推送数据
 * 5. 程序退出前调用Stop()
 */
class WebSocketStatsService {
public:
    /**
     * @brief 获取单例实例
     */
    static WebSocketStatsService& GetInstance();
    
    /**
     * @brief 启动WebSocket服务
     * 
     * @return true 启动成功
     * @return false 启动失败
     */
    bool Start();
    
    /**
     * @brief 停止WebSocket服务
     */
    void Stop();
    
    /**
     * @brief 是否正在运行
     */
    bool IsRunning() const { return running_; }
    
    /**
     * @brief 获取当前连接数
     */
    int GetConnectionCount() /*const*/;
    
    /**
     * @brief 获取发送的消息总数
     */
    int64_t GetMessagesSent() const { return messages_sent_; }
    
private:
    WebSocketStatsService();
    ~WebSocketStatsService();
    
    // 禁止拷贝
    WebSocketStatsService(const WebSocketStatsService&) = delete;
    WebSocketStatsService& operator=(const WebSocketStatsService&) = delete;
    
    // WebSocket事件处理（需要根据实际使用的WebSocket库实现）
    void OnOpen(const std::string& connection_id);
    void OnClose(const std::string& connection_id);
    void OnMessage(const std::string& connection_id, const std::string& message);
    
    // 消息处理
    void HandleSubscribe(const std::string& connection_id, const std::string& message);
    void HandleUnsubscribe(const std::string& connection_id, const std::string& message);
    void HandlePing(const std::string& connection_id);
    
    // 数据推送
    void PushStatsTimer();
    void PushSystemStats(const std::string& connection_id);
    void PushSessionsStats(const std::string& connection_id);
    void PushSessionStats(const std::string& connection_id, const std::string& session_name);
    void PushProducerStats(const std::string& connection_id, const std::string& session_name);
    void PushConsumerStats(const std::string& connection_id, const std::string& consumer_id);
    
    // 发送消息
    void SendClientMessage(const std::string& connection_id, const std::string& message);
    void SendError(const std::string& connection_id, int code, const std::string& message);
    
    // 连接管理
    ConnectionInfo* GetConnectionInfo(const std::string& connection_id);
    void RemoveConnection(const std::string& connection_id);
    
    // 工具方法
    std::string GenerateConnectionId();
    //int64_t GetCurrentTimeMs();
    
private:
    // 连接管理
    std::map<std::string, ConnectionInfo> connections_;
    std::mutex connections_mutex_;
    
    // 运行状态
    std::atomic<bool> running_;
    std::thread push_thread_;
    std::thread ping_thread_;
    
    // 统计信息
    std::atomic<int64_t> messages_sent_;
    std::atomic<int64_t> bytes_sent_;
    std::atomic<int64_t> errors_;
    
#if ENABLE_WEBSOCKET
    // WebSocket服务器实例
    std::shared_ptr<websocketpp::server<websocketpp::config::asio>> ws_server_;
    std::thread ws_thread_;
    
    // 连接句柄映射（connection_hdl <-> connection_id）
    std::map<websocketpp::connection_hdl, std::string, std::owner_less<websocketpp::connection_hdl>> hdl_to_id_;
    std::map<std::string, websocketpp::connection_hdl> id_to_hdl_;
    std::mutex hdl_map_mutex_;
#else
    // WebSocket未启用时的占位符
    void* ws_server_;
#endif
};

} // namespace gb_media_server

#endif // _GB_MEDIA_SERVER_WS_STATS_SERVICE_H_
