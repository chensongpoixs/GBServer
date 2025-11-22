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

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。

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
#include "libmedia_transfer_protocol/libflv/cflv_encoder.h"
#include "libmedia_transfer_protocol/libnetwork/connection.h"
#include "gb_media_server_log.h"


namespace  gb_media_server
{
	WebService::WebService()
		:http_server_(new libmedia_transfer_protocol::libhttp::HttpServer())
		, http_event_callback_map_()
	{
		http_server_->SignalOnRequest.connect(this, &WebService::OnRequest);
		http_server_->SignalOnDestory.connect(this, &WebService::OnDestroy);
		http_event_callback_map_["/rtc/push"] = &WebService::HandlerRtcProducer;
		// http_event_callback_map_["/rtc/pull"] = &WebService::HandlerRtcConsumer;
		http_event_callback_map_["/rtc/play"] = &WebService::HandlerRtcConsumer;
		// flv 
		http_event_callback_map_["flv"] = &WebService::HandlerFlvConsumer;

		// m3u8
		http_event_callback_map_["m3u8"] = &WebService::HandlerM3u8Consumer;

		// ts
		http_event_callback_map_["ts"] = &WebService::HandlerTsConsumer;

		//// rtp 
		http_event_callback_map_["/api/openRtpServer"] = &WebService::HandlerOpenRtpServer;
		http_event_callback_map_["/api/closeRtpServer"] = &WebService::HandlerCloseRtpServer;
		
		// RTSP
		//    创建 RTSP Producer（接收 RTSP 流）：
			// POST /rtsp/push?streamurl=app/stream
			// 创建 RTSP Consumer（发送 RTSP 流）：
           //    POST /rtsp/play?streamurl=app/stream
		//http_event_callback_map_["/rtsp/push"] = &WebService::HandlerRtspProducer;
		// http_event_callback_map_["/rtsp/pull"] = &WebService::HandlerRtspConsumer;
		//http_event_callback_map_["/rtsp/play"] = &WebService::HandlerRtspConsumer;
	
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
			case kConsumerTypeRtsp:
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
		auto flv = conn->GetContext<libmedia_transfer_protocol::libflv::FlvEncoder>(libmedia_transfer_protocol::libnetwork::kFlvContext);
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
