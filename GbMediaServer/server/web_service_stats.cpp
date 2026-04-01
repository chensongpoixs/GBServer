/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#include "server/web_service.h"
#include "share/statistics_manager.h"
#include "utils/json_utils.h"
#include "server/gb_media_service.h"
#include "rtc_base/logging.h"
#include "gb_media_server_log.h"

namespace gb_media_server {

void WebService::HandlerGetProducerStats(
    libmedia_transfer_protocol::libnetwork::Connection* conn,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
    std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> ctx)
{
    GBMEDIASERVER_LOG(LS_INFO) << "HandlerGetProducerStats: path=" << req->Path();
    
    // 从URL中提取session名称
    // URL格式: /api/stats/producer/{session}
    std::string path = req->Path();
    size_t pos = path.find("/api/stats/producer/");
    if (pos == std::string::npos) {
       // std::string response = JsonBuilder::BuildResponse(400, "Invalid URL format");
       // ctx->SendResponse(400, "Bad Request", response, "application/json");
       // GBMEDIASERVER_LOG(LS_WARNING) << "parse sdp error. session name:" << session_name;
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    std::string session_name = path.substr(pos + 20);  // 20 = strlen("/api/stats/producer/")
    if (session_name.empty()) {
        //std::string response = JsonBuilder::BuildResponse(400, "Session name is required");
        //ctx->SendResponse(400, "Bad Request", response, "application/json");

        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    // 获取Producer统计
    auto stats = StatisticsManager::GetInstance().GetProducerStatistics(session_name);
    if (!stats) {
       // std::string response = JsonBuilder::BuildResponse(404, "Producer not found");
        //ctx->SendResponse(404, "Not Found", response, "application/json");
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    // 返回统计数据
    std::string stats_json = stats->ToJson();
    http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
        auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
        res->SetStatusCode(200);
        res->AddHeader("server", "GbMediaServer");
        res->AddHeader("content-length", std::to_string(stats_json.size()));
        res->AddHeader("content-type", "application/json");
        res->AddHeader("Access-Control-Allow-Origin", "*");
        res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res->AddHeader("Allow", "POST, GET, OPTIONS");
        res->AddHeader("Access-Control-Allow-Headers", "content-type");
        res->AddHeader("Connection", "close");
        res->SetBody(stats_json);
        ctx->PostRequest(res);

        ctx->WriteComplete(conn);
        });
    //std::string response = JsonBuilder::BuildResponse(0, "success", stats_json);
   // ctx->SendResponse(200, "OK", response, "application/json");
}

void WebService::HandlerGetConsumerStats(
    libmedia_transfer_protocol::libnetwork::Connection* conn,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
    std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> ctx)
{
    GBMEDIASERVER_LOG(LS_INFO) << "HandlerGetConsumerStats: path=" << req->Path();
    
    // 从URL中提取consumer_id
    // URL格式: /api/stats/consumer/{consumer_id}
    std::string path = req->Path();
    size_t pos = path.find("/api/stats/consumer/");
    if (pos == std::string::npos) {
       // std::string response = JsonBuilder::BuildResponse(400, "Invalid URL format");
       // ctx->SendResponse(400, "Bad Request", response, "application/json");
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    std::string consumer_id = path.substr(pos + 20);  // 20 = strlen("/api/stats/consumer/")
    if (consumer_id.empty()) {
       // std::string response = JsonBuilder::BuildResponse(400, "Consumer ID is required");
       // ctx->SendResponse(400, "Bad Request", response, "application/json");
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    // 获取Consumer统计
    auto stats = StatisticsManager::GetInstance().GetConsumerStatistics(consumer_id);
    if (!stats) {
       // std::string response = JsonBuilder::BuildResponse(404, "Consumer not found");
        //ctx->SendResponse(404, "Not Found", response, "application/json");
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    // 返回统计数据
    std::string stats_json = stats->ToJson();
    http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
        auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
        res->SetStatusCode(200);
        res->AddHeader("server", "GbMediaServer");
        res->AddHeader("content-length", std::to_string(stats_json.size()));
        res->AddHeader("content-type", "application/json");
        res->AddHeader("Access-Control-Allow-Origin", "*");
        res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res->AddHeader("Allow", "POST, GET, OPTIONS");
        res->AddHeader("Access-Control-Allow-Headers", "content-type");
        res->AddHeader("Connection", "close");
        res->SetBody(stats_json);
        ctx->PostRequest(res);

        ctx->WriteComplete(conn);
        });
   // std::string response = JsonBuilder::BuildResponse(0, "success", stats_json);
   // ctx->SendResponse(200, "OK", response, "application/json");
}

void WebService::HandlerGetSessionStats(
    libmedia_transfer_protocol::libnetwork::Connection* conn,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
    std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> ctx)
{
    GBMEDIASERVER_LOG(LS_INFO) << "HandlerGetSessionStats: path=" << req->Path();
    
    // 从URL中提取session名称
    // URL格式: /api/stats/session/{session}
    std::string path = req->Path();
    size_t pos = path.find("/api/stats/session/");
    if (pos == std::string::npos) {
       /* std::string response = JsonBuilder::BuildResponse(400, "Invalid URL format");
        ctx->SendResponse(400, "Bad Request", response, "application/json");*/
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    std::string session_name = path.substr(pos + 19);  // 19 = strlen("/api/stats/session/")
    if (session_name.empty()) {
       /* std::string response = JsonBuilder::BuildResponse(400, "Session name is required");
        ctx->SendResponse(400, "Bad Request", response, "application/json");*/
        http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
            auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
            ctx->PostRequest(res);
            ctx->WriteComplete(conn);
            });
        return;
    }
    
    // 获取会话统计
    std::string stats_json = StatisticsManager::GetInstance().GetSessionStatsJson(session_name);
    //std::string response = JsonBuilder::BuildResponse(0, "success", stats_json);
   // ctx->SendResponse(200, "OK", response, "application/json");
    http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
        auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
        res->SetStatusCode(200);
        res->AddHeader("server", "GbMediaServer");
        res->AddHeader("content-length", std::to_string(stats_json.size()));
        res->AddHeader("content-type", "application/json");
        res->AddHeader("Access-Control-Allow-Origin", "*");
        res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res->AddHeader("Allow", "POST, GET, OPTIONS");
        res->AddHeader("Access-Control-Allow-Headers", "content-type");
        res->AddHeader("Connection", "close");
        res->SetBody(stats_json);
        ctx->PostRequest(res);

        ctx->WriteComplete(conn);
        });
}

void WebService::HandlerGetAllSessionsStats(
    libmedia_transfer_protocol::libnetwork::Connection* conn,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
    const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
    std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> ctx)
{
    GBMEDIASERVER_LOG(LS_INFO) << "HandlerGetAllSessionsStats";
    
    // 获取所有会话统计
    std::string stats_json = StatisticsManager::GetInstance().GetAllSessionsStatsJson();
   // std::string response = JsonBuilder::BuildResponse(0, "success", stats_json);
   // ctx->SendResponse(200, "OK", response, "application/json");
    http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
        auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
        res->SetStatusCode(200);
        res->AddHeader("server", "GbMediaServer");
        res->AddHeader("content-length", std::to_string(stats_json.size()));
        res->AddHeader("content-type", "application/json");
        res->AddHeader("Access-Control-Allow-Origin", "*");
        res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res->AddHeader("Allow", "POST, GET, OPTIONS");
        res->AddHeader("Access-Control-Allow-Headers", "content-type");
        res->AddHeader("Connection", "close");
        res->SetBody(stats_json);
        ctx->PostRequest(res);

        ctx->WriteComplete(conn);
        });
}

} // namespace gb_media_server
