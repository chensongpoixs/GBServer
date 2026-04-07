/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#include "server/ws_stats_service.h"
#include "share/statistics_manager.h"
#include "utils/yaml_config.h"
#include "gb_media_server_log.h"
#include <chrono>
#include <sstream>

namespace gb_media_server {

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief 获取WebSocketStatsService单例实例
     *
     * 使用C++11的线程安全静态局部变量实现单例模式
     *
     * @return 返回WebSocketStatsService单例实例的引用
     */
    WebSocketStatsService& WebSocketStatsService::GetInstance() {
        static WebSocketStatsService instance;
        return instance;
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief WebSocketStatsService构造函数
     *
     * 初始化所有成员变量为默认值
     */
    WebSocketStatsService::WebSocketStatsService()
        : running_(false)
        , messages_sent_(0)
        , bytes_sent_(0)
        , errors_(0)
        , ws_server_(nullptr)
    {
        GBMEDIASERVER_LOG(LS_INFO) << "WebSocketStatsService created";
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief WebSocketStatsService析构函数
     *
     * 确保服务已停止
     */
    WebSocketStatsService::~WebSocketStatsService() {
        Stop();
        GBMEDIASERVER_LOG(LS_INFO) << "WebSocketStatsService destroyed";
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief 启动WebSocket统计服务
     *
     * 工作流程：
     * 1. 检查配置是否启用
     * 2. 检查是否已经运行
     * 3. 初始化WebSocket服务器（需要websocketpp库）
     * 4. 启动推送线程
     * 5. 启动Ping线程
     *
     * @return true 启动成功，false 启动失败
     * @note 需要安装websocketpp库才能正常工作
     */
    bool WebSocketStatsService::Start() {
        // 获取配置
        const auto& config = YamlConfig::GetInstance().GetWebSocketStatsConfig();

        // 检查是否启用
        if (!config.enabled) {
            GBMEDIASERVER_LOG(LS_INFO) << "WebSocket stats service is disabled in config";
            return false;
        }

        // 检查是否已经运行
        if (running_) {
            GBMEDIASERVER_LOG(LS_WARNING) << "WebSocket stats service is already running";
            return true;
        }

        GBMEDIASERVER_LOG(LS_INFO) << "Starting WebSocket stats service on "
            << config.ip << ":" << config.port;

        // TODO: 初始化WebSocket服务器
        // 这里需要使用websocketpp或其他WebSocket库
        // 由于需要额外的依赖，这里只提供框架代码
#if ENABLE_WEBSOCKET
        ws_server_ = std::make_shared<websocketpp::server<websocketpp::config::asio>>();
        ws_server_->init_asio();
        ws_server_->set_reuse_addr(true);

        ws_server_->set_open_handler([this](websocketpp::connection_hdl hdl) {
            std::string conn_id = GenerateConnectionId();
            std::lock_guard<std::mutex> lock(hdl_map_mutex_);
            hdl_to_id_[hdl] = conn_id;
            id_to_hdl_[conn_id] = hdl;
            OnOpen(conn_id);
            });
        websocketpp::lib::error_code  ec ;
        ws_server_->listen(config.port, ec);
        ws_server_->start_accept();

        ws_thread_ = std::thread([this]() {
            ws_server_->run();
            });
#endif
        // 标记为运行状态
        running_ = true;

        // 启动推送线程
        push_thread_ = std::thread([this, config]() {
            GBMEDIASERVER_LOG(LS_INFO) << "Push thread started";
            while (running_) {
                PushStatsTimer();
                std::this_thread::sleep_for(std::chrono::milliseconds(config.push_interval));
            }
            GBMEDIASERVER_LOG(LS_INFO) << "Push thread stopped";
            });

        // 启动Ping线程
        ping_thread_ = std::thread([this, config]() {
            GBMEDIASERVER_LOG(LS_INFO) << "Ping thread started";
            while (running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(config.ping_interval));

                // 检查所有连接的Ping超时
                std::lock_guard<std::mutex> lock(connections_mutex_);
                int64_t current_time = GetCurrentTimeMs();

                for (auto& pair : connections_) {
                    auto& conn_info = pair.second;
                    if (current_time - conn_info.last_ping_time > config.ping_timeout) {
                        GBMEDIASERVER_LOG(LS_WARNING) << "Connection " << pair.first
                            << " ping timeout, closing";
                        // TODO: 关闭超时连接
#if ENABLE_WEBSOCKET
                        std::lock_guard<std::mutex> hdl_lock(hdl_map_mutex_);
                        auto hdl_it = id_to_hdl_.find(pair.first);
                        if (hdl_it != id_to_hdl_.end() && ws_server_) {
                            ws_server_->close(hdl_it->second,
                                websocketpp::close::status::going_away,
                                "Ping timeout");
                        }
#endif
                    }
                }
            }
            GBMEDIASERVER_LOG(LS_INFO) << "Ping thread stopped";
            });

        GBMEDIASERVER_LOG(LS_INFO) << "WebSocket stats service started successfully";
        return true;
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief 停止WebSocket统计服务
     *
     * 工作流程：
     * 1. 设置运行标志为false
     * 2. 等待推送线程结束
     * 3. 等待Ping线程结束
     * 4. 关闭所有连接
     * 5. 停止WebSocket服务器
     */
    void WebSocketStatsService::Stop() {
        if (!running_) {
            return;
        }

        GBMEDIASERVER_LOG(LS_INFO) << "Stopping WebSocket stats service...";

        // 设置运行标志为false
        running_ = false;

        // 等待推送线程结束
        if (push_thread_.joinable()) {
            push_thread_.join();
        }

        // 等待Ping线程结束
        if (ping_thread_.joinable()) {
            ping_thread_.join();
        }

        // 关闭所有连接
        {
            std::lock_guard<std::mutex> lock(connections_mutex_);
            connections_.clear();
        }

        // TODO: 停止WebSocket服务器
#if ENABLE_WEBSOCKET
        if (ws_server_) {
            ws_server_->stop_listening();

            {
                std::lock_guard<std::mutex> lock(hdl_map_mutex_);
                for (auto& pair : id_to_hdl_) {
                    ws_server_->close(pair.second,
                        websocketpp::close::status::going_away,
                        "Server shutting down");
                }
                hdl_to_id_.clear();
                id_to_hdl_.clear();
            }

            ws_server_->stop();
            if (ws_thread_.joinable()) {
                ws_thread_.join();
            }
            ws_server_.reset();
        }
#endif
        GBMEDIASERVER_LOG(LS_INFO) << "WebSocket stats service stopped. Stats: "
            << "messages_sent=" << messages_sent_
            << ", bytes_sent=" << bytes_sent_
            << ", errors=" << errors_;
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief 获取当前连接数
     *
     * @return 当前连接数
     */
    int WebSocketStatsService::GetConnectionCount()  /*const*/ {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        return static_cast<int>(connections_.size());
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief WebSocket连接打开事件处理
     *
     * @param connection_id 连接ID
     */
    void WebSocketStatsService::OnOpen(const std::string& connection_id) {
        GBMEDIASERVER_LOG(LS_INFO) << "WebSocket connection opened: " << connection_id;

        std::lock_guard<std::mutex> lock(connections_mutex_);

        ConnectionInfo conn_info;
        conn_info.connection_id = connection_id;
        conn_info.last_ping_time = GetCurrentTimeMs();
        conn_info.authenticated = false;
        conn_info.user_data = nullptr;

        connections_[connection_id] = conn_info;
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief WebSocket连接关闭事件处理
     *
     * @param connection_id 连接ID
     */
    void WebSocketStatsService::OnClose(const std::string& connection_id) {
        GBMEDIASERVER_LOG(LS_INFO) << "WebSocket connection closed: " << connection_id;

        RemoveConnection(connection_id);
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief WebSocket消息接收事件处理
     *
     * 支持的消息类型：
     * - subscribe: 订阅统计数据
     * - unsubscribe: 取消订阅
     * - ping: 心跳消息
     *
     * @param connection_id 连接ID
     * @param message 消息内容（JSON格式）
     */
    void WebSocketStatsService::OnMessage(const std::string& connection_id, const std::string& message) {
        GBMEDIASERVER_LOG(LS_VERBOSE) << "WebSocket message received from " << connection_id
            << ": " << message;

        // TODO: 解析JSON消息
        // 这里需要使用nlohmann/json或其他JSON库
        // 简化版本：根据消息内容判断类型
#if ENABLE_WEBSOCKET
        try {
            auto json_msg = nlohmann::json::parse(message);

            if (!json_msg.contains("type")) {
                SendError(connection_id, 400, "Missing 'type' field");
                return;
            }

            std::string msg_type = json_msg["type"].get<std::string>();

            if (msg_type == "subscribe") {
                HandleSubscribe(connection_id, message);
            }
            else if (msg_type == "unsubscribe") {
                HandleUnsubscribe(connection_id, message);
            }
            else if (msg_type == "ping") {
                HandlePing(connection_id);
            }
            else {
                SendError(connection_id, 400, "Unknown message type: " + msg_type);
            }

        }
        catch (const nlohmann::json::parse_error& e) {
            SendError(connection_id, 400, "Invalid JSON format");
        }
        catch (const std::exception& e) {
            SendError(connection_id, 500, "Internal server error");
        }
#endif
        /*if (message.find("\"type\":\"subscribe\"") != std::string::npos) {
            HandleSubscribe(connection_id, message);
        } else if (message.find("\"type\":\"unsubscribe\"") != std::string::npos) {
            HandleUnsubscribe(connection_id, message);
        } else if (message.find("\"type\":\"ping\"") != std::string::npos) {
            HandlePing(connection_id);
        } else {
            SendError(connection_id, 400, "Unknown message type");
        }*/
    }

    /**
     * @author chensong
     * @date 2025-10-18
     * @brief 处理订阅请求
     *
     * @param connection_id 连接ID
     * @param message 订阅消息（JSON格式）
     */
    void WebSocketStatsService::HandleSubscribe(const std::string& connection_id, const std::string& message) {
        GBMEDIASERVER_LOG(LS_INFO) << "Handle subscribe from " << connection_id;

        // TODO: 解析订阅参数
        // 简化版本：默认订阅系统统计
#if ENABLE_WEBSOCKET
        try {
            auto json_msg = nlohmann::json::parse(message);

            if (!json_msg.contains("target")) {
                SendError(connection_id, 400, "Missing 'target' field");
                return;
            }

            std::string target_str = json_msg["target"].get<std::string>();
            std::string id = json_msg.value("id", "");
            int64_t interval = json_msg.value("interval", 1000);

            // 解析订阅目标类型
            SubscriptionTarget target;
            if (target_str == "system") {
                target = SubscriptionTarget::SYSTEM;
            }
            else if (target_str == "session") {
                target = SubscriptionTarget::SESSION;
                if (id.empty()) {
                    SendError(connection_id, 400, "Missing 'id' for session subscription");
                    return;
                }
            }
            // ... 其他类型
            auto* conn_info = GetConnectionInfo(connection_id);
            if (conn_info) {
                // 添加订阅
                Subscription sub;
                sub.target = target;
                sub.id = id;
                sub.interval = interval;
                sub.last_push_time = 0;
                conn_info->subscriptions.push_back(sub);
            }
            // 发送确认
            nlohmann::json response;
            response["type"] = "subscribed";
            response["target"] = target_str;
            if (!id.empty()) {
                response["id"] = id;
            }
            SendClientMessage(connection_id, response.dump());
        }
        catch (const nlohmann::json::parse_error& e) {
            SendError(connection_id, 400, "Invalid JSON format");
        }
        catch (const std::exception& e) {
            SendError(connection_id, 500, "Internal server error");
        }
#endif
        /* std::lock_guard<std::mutex> lock(connections_mutex_);
         auto* conn_info = GetConnectionInfo(connection_id);
         if (conn_info) {
             Subscription sub;
             sub.target = SubscriptionTarget::SYSTEM;
             sub.id = "";
             sub.interval = 1000;
             sub.last_push_time = 0;

             conn_info->subscriptions.push_back(sub);

             GBMEDIASERVER_LOG(LS_INFO) << "Subscription added for " << connection_id;
         }*/
   // }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 处理取消订阅请求
 * 
 * @param connection_id 连接ID
 * @param message 取消订阅消息（JSON格式）
 */
void WebSocketStatsService::HandleUnsubscribe(const std::string& connection_id, const std::string& message) {
    GBMEDIASERVER_LOG(LS_INFO) << "Handle unsubscribe from " << connection_id;
    
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto* conn_info = GetConnectionInfo(connection_id);
    if (conn_info) {
        conn_info->subscriptions.clear();
        GBMEDIASERVER_LOG(LS_INFO) << "All subscriptions removed for " << connection_id;
    }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 处理Ping请求
 * 
 * @param connection_id 连接ID
 */
void WebSocketStatsService::HandlePing(const std::string& connection_id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto* conn_info = GetConnectionInfo(connection_id);
    if (conn_info) {
        conn_info->last_ping_time = GetCurrentTimeMs();
        
        // 发送Pong响应
        std::string pong_message = "{\"type\":\"pong\",\"timestamp\":" + 
                                   std::to_string(GetCurrentTimeMs()) + "}";
        SendClientMessage(connection_id, pong_message);
    }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 定时推送统计数据
 * 
 * 遍历所有连接，检查订阅并推送数据
 */
void WebSocketStatsService::PushStatsTimer() {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    int64_t current_time = GetCurrentTimeMs();
    
    for (auto& pair : connections_) {
        const std::string& connection_id = pair.first;
        ConnectionInfo& conn_info = pair.second;
        
        for (auto& sub : conn_info.subscriptions) {
            // 检查是否到达推送时间
            if (current_time - sub.last_push_time >= sub.interval) {
                // 根据订阅类型推送数据
                switch (sub.target) {
                    case SubscriptionTarget::SYSTEM:
                        PushSystemStats(connection_id);
                        break;
                    case SubscriptionTarget::SESSIONS:
                        PushSessionsStats(connection_id);
                        break;
                    case SubscriptionTarget::SESSION:
                        PushSessionStats(connection_id, sub.id);
                        break;
                    case SubscriptionTarget::PRODUCER:
                        PushProducerStats(connection_id, sub.id);
                        break;
                    case SubscriptionTarget::CONSUMER:
                        PushConsumerStats(connection_id, sub.id);
                        break;
                }
                
                sub.last_push_time = current_time;
            }
        }
    }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 推送系统统计数据
 * 
 * @param connection_id 连接ID
 */
void WebSocketStatsService::PushSystemStats(const std::string& connection_id) {
    try {
        std::string stats_json = StatisticsManager::GetInstance().GetSystemStatsJson();
        
        // 构造响应消息
        std::ostringstream oss;
        oss << "{\"type\":\"stats\",\"target\":\"system\",\"timestamp\":" 
            << GetCurrentTimeMs() << ",\"data\":" << stats_json << "}";
        
        SendClientMessage(connection_id, oss.str());
        
    } catch (const std::exception& e) {
        GBMEDIASERVER_LOG(LS_ERROR) << "Failed to push system stats: " << e.what();
        errors_++;
    }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 推送会话列表统计数据
 * 
 * @param connection_id 连接ID
 */
void WebSocketStatsService::PushSessionsStats(const std::string& connection_id) {
    try {
        std::string stats_json = StatisticsManager::GetInstance().GetAllSessionsStatsJson();
        
        std::ostringstream oss;
        oss << "{\"type\":\"stats\",\"target\":\"sessions\",\"timestamp\":" 
            << GetCurrentTimeMs() << ",\"data\":" << stats_json << "}";
        
        SendClientMessage(connection_id, oss.str());
        
    } catch (const std::exception& e) {
        GBMEDIASERVER_LOG(LS_ERROR) << "Failed to push sessions stats: " << e.what();
        errors_++;
    }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 推送单个会话统计数据
 * 
 * @param connection_id 连接ID
 * @param session_name 会话名称
 */
void WebSocketStatsService::PushSessionStats(const std::string& connection_id, const std::string& session_name) {
    try {
        std::string stats_json = StatisticsManager::GetInstance().GetSessionStatsJson(session_name);
        
        std::ostringstream oss;
        oss << "{\"type\":\"stats\",\"target\":\"session\",\"id\":\"" << session_name 
            << "\",\"timestamp\":" << GetCurrentTimeMs() << ",\"data\":" << stats_json << "}";
        
        SendClientMessage(connection_id, oss.str());
        
    } catch (const std::exception& e) {
        GBMEDIASERVER_LOG(LS_ERROR) << "Failed to push session stats: " << e.what();
        errors_++;
    }
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 推送Producer统计数据
 * 
 * @param connection_id 连接ID
 * @param session_name 会话名称
 */
void WebSocketStatsService::PushProducerStats(const std::string& connection_id, const std::string& session_name) {
    // TODO: 实现Producer统计推送
    GBMEDIASERVER_LOG(LS_VERBOSE) << "Push producer stats for " << session_name;
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 推送Consumer统计数据
 * 
 * @param connection_id 连接ID
 * @param consumer_id Consumer ID
 */
void WebSocketStatsService::PushConsumerStats(const std::string& connection_id, const std::string& consumer_id) {
    // TODO: 实现Consumer统计推送
    GBMEDIASERVER_LOG(LS_VERBOSE) << "Push consumer stats for " << consumer_id;
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 发送消息到客户端
 * 
 * @param connection_id 连接ID
 * @param message 消息内容
 */
void WebSocketStatsService::SendClientMessage(const std::string& connection_id, const std::string& message) {
    // TODO: 使用WebSocket库发送消息
    // 这里需要实际的WebSocket发送实现
    
    messages_sent_++;
    bytes_sent_ += message.size();
    
    GBMEDIASERVER_LOG(LS_VERBOSE) << "Send message to " << connection_id 
                                  << ", size=" << message.size();
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 发送错误消息到客户端
 * 
 * @param connection_id 连接ID
 * @param code 错误代码
 * @param message 错误消息
 */
void WebSocketStatsService::SendError(const std::string& connection_id, int code, const std::string& message) {
    std::ostringstream oss;
    oss << "{\"type\":\"error\",\"code\":" << code 
        << ",\"message\":\"" << message << "\"}";
    
    SendClientMessage(connection_id, oss.str());
    errors_++;
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 获取连接信息
 * 
 * @param connection_id 连接ID
 * @return 连接信息指针，如果不存在返回nullptr
 * @note 调用者需要持有connections_mutex_锁
 */
ConnectionInfo* WebSocketStatsService::GetConnectionInfo(const std::string& connection_id) {
    auto it = connections_.find(connection_id);
    if (it != connections_.end()) {
        return &it->second;
    }
    return nullptr;
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 移除连接
 * 
 * @param connection_id 连接ID
 */
void WebSocketStatsService::RemoveConnection(const std::string& connection_id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.erase(connection_id);
    GBMEDIASERVER_LOG(LS_INFO) << "Connection removed: " << connection_id 
                               << ", remaining: " << connections_.size();
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 生成连接ID
 * 
 * @return 唯一的连接ID
 */
std::string WebSocketStatsService::GenerateConnectionId() {
    static std::atomic<int64_t> counter(0);
    return "conn_" + std::to_string(GetCurrentTimeMs()) + "_" + std::to_string(counter++);
}

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 获取当前时间（毫秒）
 * 
 * @return 当前时间戳（毫秒）
 */
int64_t WebSocketStatsService::GetCurrentTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

} // namespace gb_media_server
