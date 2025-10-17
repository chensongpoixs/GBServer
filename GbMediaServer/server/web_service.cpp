/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-17



 ******************************************************************************/
#include "server/web_service.h"
#include "json/json.h"
#include "libmedia_transfer_protocol/libhttp/http_context.h"
#include "libmedia_transfer_protocol/libhttp/tcp_session.h"
#include "utils/string_utils.h"
#include "server/connection.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "utils/string_utils.h"
namespace  gb_media_server
{
	WebService::WebService()
		:http_server_(new libmedia_transfer_protocol::libhttp::HttpServer())
	{
		http_server_->SignalOnRequest.connect(this, &WebService::OnRequest);
	}
	WebService::~WebService()
	{
		if (http_server_)
		{
			http_server_->SignalOnRequest.disconnect(this);
		}
	}
	bool WebService::StartWebServer(const char * ip, uint16_t port)
	{
		return http_server_->Startup(ip, port);
	}
	void WebService::OnRequest(libmedia_transfer_protocol::libhttp::TcpSession * conn, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet)
	{
		auto  http_ctx = conn->GetContext<libmedia_transfer_protocol::libhttp::HttpContext>(libmedia_transfer_protocol::libhttp::kHttpContext);
		if (!http_ctx)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "no found http context.something must be wrong.";
			return;
		}
		if (req->IsRequest())
		{
			GBMEDIASERVER_LOG(LS_INFO) << "req method:" << req->Method() << " path:" << req->Path();
		}
		else
		{
			GBMEDIASERVER_LOG(LS_INFO) << "req code:" << req->GetStatusCode() << " msg:" << libmedia_transfer_protocol::libhttp::HttpUtils::ParseStatusMessage(req->GetStatusCode());
		}

		auto headers = req->Headers();
		for (auto const &h : headers)
		{
			GBMEDIASERVER_LOG(LS_INFO) << h.first << ":" << h.second;
		}

		if (req->IsRequest())
		{
			if (req->Method() == libmedia_transfer_protocol::libhttp::kOptions)
			{
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttpOptionsResponse();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
				return;
			}
			if (req->Path() == "/rtc/play/" && packet)
			{
				GBMEDIASERVER_LOG(LS_INFO) << "request:\n" << packet->Data();
				Json::CharReaderBuilder builder;
				Json::Value root;
				Json::String err;
				std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				if (!reader->parse(packet->Data(), packet->Data() + packet->PacketSize(), &root, &err))
				{
					GBMEDIASERVER_LOG(LS_WARNING) << "parse json error.";
					auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
					http_ctx->PostRequest(res);
					http_ctx->WriteComplete(conn);
					return;
				}

				auto type = root["type"].asString();
				auto streamurl = root["streamurl"].asString();
				auto clientip = root["clientid"].asString();
				auto sdp = root["sdp"].asString();
				auto capture_value = root["caputretype"];
				

				std::string session_name = string_utils:: GetSessionNameFromUrl(streamurl);
				GBMEDIASERVER_LOG(LS_INFO) << "get session name:" << session_name;

				GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [=]() {
					auto s = GbMediaService::GetInstance().CreateSession(session_name);
					if (!s)
					{
						GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << session_name;
						http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
							auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
							http_ctx->PostRequest(res);
							http_ctx->WriteComplete(conn);
						});

						return;
					}
					// rtc::SocketAddress   server_addrs;
					// server_addrs.SetIP("192.168.1.2");
					// server_addrs.SetPort(10003);
					// auto socket = GbMediaService::GetInstance().network_thread()->socketserver()->CreateSocket(server_addrs.ipaddr().family(), SOCK_STREAM);
					auto  connection = std::make_shared<Connection>(conn->GetSocket());
					auto user = s->CreatePlayerUser(connection, session_name, "", UserType::kUserTypePlayerWebRTC);

					if (!user)
					{
						GBMEDIASERVER_LOG(LS_WARNING) << "cant create user session  name:" << session_name;
						// auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
						// http_ctx->PostRequest(res);
						http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
							auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
							http_ctx->PostRequest(res);
							http_ctx->WriteComplete(conn);
						});
						return;
					}

					s->AddPlayer(std::dynamic_pointer_cast<PlayerUser>(user));

					PlayRtcUserPtr rtc_user = std::dynamic_pointer_cast<PlayRtcUser>(user);
					if (!rtc_user->ProcessOfferSdp(sdp))
					{
						GBMEDIASERVER_LOG(LS_WARNING) << "parse sdp error. session name:" << session_name;
						http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
							auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
							http_ctx->PostRequest(res);
							http_ctx->WriteComplete(conn);
						});
						return;
					}

					auto answer_sdp = rtc_user->BuildAnswerSdp();
					GBMEDIASERVER_LOG(LS_INFO) << " answer sdp:" << answer_sdp;

					Json::Value result;
					result["code"] = 0;
					result["server"] = "WebServer";
					result["type"] = "answer";
					result["sdp"] = std::move(answer_sdp);
					result["sessionid"] = rtc_user->RemoteUFrag() + ":" + rtc_user->LocalUFrag();

					auto content = result.toStyledString();
					http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
						auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
						res->SetStatusCode(200);
						res->AddHeader("server", "WebServer");
						res->AddHeader("content-length", std::to_string(content.size()));
						res->AddHeader("content-type", "text/plain");
						res->AddHeader("Access-Control-Allow-Origin", "*");
						res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
						res->AddHeader("Allow", "POST, GET, OPTIONS");
						res->AddHeader("Access-Control-Allow-Headers", "content-type");
						res->AddHeader("Connection", "close");
						res->SetBody(content);
						http_ctx->PostRequest(res);

						http_ctx->WriteComplete(conn);
					});
					//采集桌面的画面
					if (!capture_value.isNull()&&   capture_value.isInt())
					{
						rtc_user->SetCapture(capture_value.asInt() > 0 ? true: false);
					}
					//GbMediaService::GetInstance().GetRtcServer()->A
					RtcService::GetInstance().AddPlayUser(rtc_user);

				});

			}
		}
	}
	void WebService::HandlerRtcPlay()
	{
	}
}
