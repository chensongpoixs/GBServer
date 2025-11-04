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
#include "libmedia_transfer_protocol/libnetwork/tcp_session.h"
#include "utils/string_utils.h"
#include "server/gb_media_service.h"
#include "server/rtc_service.h"
#include "utils/string_utils.h"
#include "consumer/rtc_consumer.h"
#include "producer/gb28181_producer.h"
#include "libmedia_transfer_protocol/libflv/cflv_context.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
namespace  gb_media_server
{
	WebService::WebService()
		:http_server_(new libmedia_transfer_protocol::libhttp::HttpServer())
		, http_event_callback_map_()
	{
		http_server_->SignalOnRequest.connect(this, &WebService::OnRequest);
		http_server_->SignalOnDestory.connect(this, &WebService::OnDestroy);

		http_event_callback_map_["/rtc/play"] = &WebService::HandlerRtcConsumer;
		http_event_callback_map_["/rtc/push"] = &WebService::HandlerRtcProducer;

		http_event_callback_map_["/api/openRtpServer"] = &WebService::HandlerOpenRtpServer;
		http_event_callback_map_["/api/closeRtpServer"] = &WebService::HandlerCloseRtpServer;
		http_event_callback_map_["flv"] = &WebService::HandlerFlvConsumer;
	
	}
	WebService::~WebService()
	{
		if (http_server_)
		{
			http_server_->SignalOnRequest.disconnect(this);
			http_server_->SignalOnDestory.disconnect(this);
		}
	}
	bool WebService::StartWebServer(const char * ip, uint16_t port)
	{
		return http_server_->Startup(ip, port);
	}
	void WebService::OnRequest(libmedia_transfer_protocol::libnetwork::Connection * conn,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet)
	{
		std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext>  http_ctx = 
			conn->GetContext<libmedia_transfer_protocol::libhttp::HttpContext>(libmedia_transfer_protocol::libnetwork::kHttpContext);
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
					std::string ext = string_utils::FileExt(req->Path());
					GBMEDIASERVER_LOG(LS_INFO) << "ext:" << ext;
					event_iter = http_event_callback_map_.find(ext);
					if (event_iter != http_event_callback_map_.end())
					{
						(this->*(event_iter->second))(conn, req, packet, http_ctx);
					}
					else
					{
						RTC_LOG(LS_WARNING) << "http  event callback path:" << req->Path();
					}
				}

					
 

			}); 
		}
	}
	void WebService::OnDestroy(libmedia_transfer_protocol::libnetwork::Connection * conn)
	{
		auto shareResource = conn->GetContext<ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
		if (shareResource)
		{
			switch (shareResource->ShareResouceType())
			{
			case kProducerTypeGB28181:
			{
				//shareResource->GetSession()->SetProducer(nullptr);
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << " producer  share resource type :" << shareResource->ShareResouceType();
				break;
			}
			case kConsumerTypeRTC:
			case kConsumerTypeFlv:
			{
				shareResource->GetSession()->RemoveConsumer(std::dynamic_pointer_cast<Consumer>(shareResource));
				break;
			}
			default:
				GBMEDIASERVER_LOG_T_F(LS_WARNING) << " share resource type :" << shareResource->ShareResouceType();
				break;
			}
			gb_media_server::GbMediaService::GetInstance().worker_thread()->Invoke<void>(RTC_FROM_HERE, [&]() {
				conn->ClearContext(libmedia_transfer_protocol::libnetwork::kShareResourceContext);
			});
			
			 
		}
		auto flv = conn->GetContext<libmedia_transfer_protocol::libflv::FlvContext>(libmedia_transfer_protocol::libnetwork::kFlvContext);
		if (flv)
		{ 
			gb_media_server::GbMediaService::GetInstance().worker_thread()->Invoke<void>(RTC_FROM_HERE, [& ]() {
				conn->ClearContext(libmedia_transfer_protocol::libnetwork::kFlvContext);
			});
			 
		}
	}
	
	//void WebService::HandlerRtcPlay(libmedia_transfer_protocol::libhttp::TcpSession * conn,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
	//	std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> ctx)
	//{
	//}
}
