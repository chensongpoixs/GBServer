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
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "utils/string_utils.h"
#include "consumer/rtc_play_consumer.h"
#include "producer/gb28181_push_producer.h"
namespace  gb_media_server
{
	WebService::WebService()
		:http_server_(new libmedia_transfer_protocol::libhttp::HttpServer())
		, http_event_callback_map_()
	{
		http_server_->SignalOnRequest.connect(this, &WebService::OnRequest);


		http_event_callback_map_["/rtc/play"] = &WebService::HandlerRtcConsumer;
		http_event_callback_map_["/api/openRtpServer"] = &WebService::HandlerOpenRtpServer;
		http_event_callback_map_["/api/closeRtpServer"] = &WebService::HandlerCloseRtpServer;

	
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
		std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext>  http_ctx = 
			conn->GetContext<libmedia_transfer_protocol::libhttp::HttpContext>(libmedia_transfer_protocol::libhttp::kHttpContext);
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
			GbMediaService::GetInstance().worker_thread()->PostTask(RTC_FROM_HERE, [=]() {

				auto event_iter = http_event_callback_map_.find(req->Path());
				if (event_iter != http_event_callback_map_.end())
				{
					(this->*(event_iter->second))(conn, req, packet, http_ctx);
				}
				else
				{
					RTC_LOG(LS_WARNING) << "http  event callback path:" << req->Path();

				}

			});
			//if (req->Path() == "/rtc/play/" && packet)
			//{
			//	
			//
			//}
		}
	}
	void WebService::HandlerRtcConsumer(libmedia_transfer_protocol::libhttp::TcpSession * conn,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
		std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
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


		std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
		GBMEDIASERVER_LOG(LS_INFO) << "get session name:" << session_name;

		
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
			std::shared_ptr<RtcPlayConsumer> consumer = std::dynamic_pointer_cast<RtcPlayConsumer>(s->CreateConsumer(
				session_name, "", ShareResourceType::kConsumerTypePlayerWebRTC));

			if (!consumer)
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "cant create consumer session  name:" << session_name;
				// auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				// http_ctx->PostRequest(res);
				http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
					auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
					http_ctx->PostRequest(res);
					http_ctx->WriteComplete(conn);
				});
				return;
			}
			consumer->SetRemoteAddress(conn->GetSocket()->GetRemoteAddress());
			GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
			s->AddConsumer( (consumer));
			GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
			//PlayRtcUserPtr rtc_user = std::dynamic_pointer_cast<PlayRtcUser>(user);
			if (!consumer->ProcessOfferSdp(sdp))
			{
				GBMEDIASERVER_LOG(LS_WARNING) << "parse sdp error. session name:" << session_name;
				http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
					auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
					http_ctx->PostRequest(res);
					http_ctx->WriteComplete(conn);
				});
				return;
			}

			auto answer_sdp = consumer->BuildAnswerSdp();
			GBMEDIASERVER_LOG(LS_INFO) << " answer sdp:" << answer_sdp;
			
			consumer->MayRunDtls();
			Json::Value result;
			result["code"] = 0;
			result["server"] = "WebServer";
			result["type"] = "answer";
			result["sdp"] = std::move(answer_sdp);
			result["sessionid"] = consumer->RemoteUFrag() + ":" + consumer->LocalUFrag();

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
			
			GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
			//采集桌面的画面
			if (!capture_value.isNull() && capture_value.isInt())
			{
				consumer->SetCapture(capture_value.asInt() > 0 ? true : false);
			}
			//GbMediaService::GetInstance().GetRtcServer()->A
			RtcService::GetInstance().AddConsumer(consumer);
			GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
		//});
	}
	void WebService::HandlerOpenRtpServer(libmedia_transfer_protocol::libhttp::TcpSession * conn, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet, 
		std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
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
			//conn->Close();
			return;
		}

		auto port = root["port"].asUInt();
		auto tcp_mode = root["tcpmode"].asUInt();
		auto stream_id = root["streamid"].asString(); 

		std::string session_name = "chensong.com/live/" + stream_id;
		//std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
		GBMEDIASERVER_LOG(LS_INFO) << "open rtp port:" << port << ", tcp_mode: " << tcp_mode << ", stream_id: " << stream_id; // session name : " << session_name;


		auto s = GbMediaService::GetInstance().CreateSession(session_name);
		if (!s)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "cant create session  name:" << stream_id;
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
			//	conn->Close();
			});

			return;
		}
		static uint16_t tcp_port = 20000;

		auto rtp = GbMediaService::GetInstance().OpenTcpServer(session_name, tcp_port);
		if (!rtp)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "cant create rtp server failed  session  name:" << session_name;
			// auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
			// http_ctx->PostRequest(res);
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
				//	conn->Close();
			});
			return;
		}
		//auto  connection = std::make_shared<Connection>(conn->GetSocket());
		auto producer = s->CreateProducer(  session_name, "", ShareResourceType::kProducerTypePublishGB28181);

		if (!producer)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "cant create producer session  name:" << stream_id;
			// auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
			// http_ctx->PostRequest(res);
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
			//	conn->Close();
			});
			return;
		}
	//	++port;
		s->SetProducer(producer);

		rtp->SetContext(libmedia_transfer_protocol::libhttp::kUserContext, producer);
 
		
		Json::Value result;
		result["code"] = 0;
		result["tcpmode"] = tcp_mode;
		result["streamid"] = stream_id;
		result["port"] =   tcp_port;
		++tcp_port;

		auto content = result.toStyledString();
		GBMEDIASERVER_LOG(LS_INFO) << " open rtp server info :" << content;

		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
			auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			res->SetStatusCode(200);
			res->AddHeader("server", "WebServer");
			res->AddHeader("content-length", std::to_string(content.size()));
			res->AddHeader("content-type", "application/json; charset=UTF-8");
			res->AddHeader("Access-Control-Allow-Origin", "*");
			res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
			res->AddHeader("Allow", "POST, GET, OPTIONS");
			res->AddHeader("Access-Control-Allow-Headers", "content-type");
			res->AddHeader("Connection", "close");
			res->SetBody(content);
			http_ctx->PostRequest(res);

			http_ctx->WriteComplete(conn);
			//conn->Close();
		});
		 
	}
	void WebService::HandlerCloseRtpServer(libmedia_transfer_protocol::libhttp::TcpSession * conn, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet, 
		std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
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

		//auto port = root["port"].asUInt();
		//auto tcp_mode = root["tcpmode"].asUInt();
		auto stream_id = root["streamid"].asString();

		std::string session_name = "chensong.com/live/" + stream_id;
		//std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
		GBMEDIASERVER_LOG(LS_INFO) << "close rtp  stream_id: " << stream_id; // session name : " << session_name;


		auto ok = GbMediaService::GetInstance().CloseSession(session_name);
		if (!ok)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "cant  rtp server not find stream  name:" << stream_id;
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
			});

			return;
		}
	 
		Json::Value result;
		result["code"] = 0; 
		result["streamid"] = stream_id;



		auto content = result.toStyledString();
		GBMEDIASERVER_LOG(LS_INFO) << " close rtp server info :" << content;

		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
			auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			res->SetStatusCode(200);
			res->AddHeader("server", "WebServer");
			res->AddHeader("content-length", std::to_string(content.size()));
			res->AddHeader("content-type", "application/json");
			res->AddHeader("Access-Control-Allow-Origin", "*");
			res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
			res->AddHeader("Allow", "POST, GET, OPTIONS");
			res->AddHeader("Access-Control-Allow-Headers", "content-type");
			res->AddHeader("Connection", "close");
			res->SetBody(content);
			http_ctx->PostRequest(res);

			http_ctx->WriteComplete(conn);
		});
	}
	//void WebService::HandlerRtcPlay(libmedia_transfer_protocol::libhttp::TcpSession * conn,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
	//	std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx)
	//{
	//}
}
