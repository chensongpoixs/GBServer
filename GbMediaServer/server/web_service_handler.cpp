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
				   date:  2025-11-03

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

#include "producer/rtc_producer.h"
#include "producer/crtsp_producer.h"
#include "consumer/crtsp_consumer.h"

#include "gb_media_server_log.h"

#include "libmedia_transfer_protocol/libhls/chls_muxer.h"
#include "libmedia_transfer_protocol/libmpeg/packet.h"
namespace gb_media_server
{



	void WebService::HandlerRtcProducer(libmedia_transfer_protocol::libnetwork::Connection* conn,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
		std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "request:" << packet->Data();
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

		if (!root.isMember("streamurl") ||
			!root.isMember("sdp"))
		{
			GBMEDIASERVER_LOG(LS_WARNING) << " json error.";
			Json::Value result;
			result["code"] = 300; 
			 
			result["message"] = "json not find sdp  or  streamurl ";
			 
			auto content = result.toStyledString();
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				res->SetStatusCode(200);
				res->AddHeader("server", "GbMediaServer");
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
			return;
		}

		//auto type = root["type"].asString();
		auto streamurl = root["streamurl"].asString();
		//auto clientip = root["clientid"].asString();
		auto sdp = root["sdp"].asString();
		//auto capture_value = root["caputretype"];


		std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
		GBMEDIASERVER_LOG(LS_INFO) << "get session name:" << session_name
			<< ", sdp:" << sdp;


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
		 
		std::shared_ptr<RtcProducer> producer = std::dynamic_pointer_cast<RtcProducer>(s->CreateProducer(
			session_name, "", ShareResourceType::kProducerTypeRtc));

		if (!producer)
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "cant create rtc producer session  name:" << session_name;
			// auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
			// http_ctx->PostRequest(res);
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
				});
			return;
		}
		//producer->SetRtcRemoteAddress(conn->GetSocket()->GetRemoteAddress());
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player producer : count : " << producer.use_count();
		
		s->SetProducer(producer);
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player producer : count : " << producer.use_count();
		 
		if (!producer->ProcessOfferSdp(libmedia_transfer_protocol::librtc::kRtcSdpPush, sdp))
		{
			GBMEDIASERVER_LOG(LS_WARNING) << "parse sdp error. session name:" << session_name;
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				http_ctx->PostRequest(res);
				http_ctx->WriteComplete(conn);
				});
			return;
		}

		auto answer_sdp = producer->BuildAnswerSdp();
		GBMEDIASERVER_LOG(LS_INFO) << "producer answer sdp:" << answer_sdp;

		producer->MayRunDtls();
		Json::Value result;
		result["code"] = 0;
		result["server"] = "WebServer";
		result["type"] = "answer";
		result["sdp"] = std::move(answer_sdp);
		result["sessionid"] = producer->RemoteUFrag() + ":" + producer->LocalUFrag();

		auto content = result.toStyledString();
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

		GBMEDIASERVER_LOG(LS_INFO) << "rtc player producer : count : " << producer.use_count();
		 
		//�������Ϊ stun����ʱ��Ҫ��֤�û��������� ������Ҫ�ֵ�ȫ����rtc����������ȥ
		RtcService::GetInstance().RegisterRtcInterface(producer);
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player producer : count : " << producer.use_count();

	}


	void WebService::HandlerRtcConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
		std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "request:" << packet->Data();
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
		if (!root.isMember("streamurl") ||
			!root.isMember("sdp"))
		{
			GBMEDIASERVER_LOG(LS_WARNING) << " json error.";
			Json::Value result;
			result["code"] = 300;

			result["message"] = "json not find sdp  or  streamurl ";

			auto content = result.toStyledString();
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				res->SetStatusCode(200);
				res->AddHeader("server", "GbMediaServer");
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
			return;
		}
		//auto type = root["type"].asString();
		auto streamurl = root["streamurl"].asString();
		//auto clientip = root["clientid"].asString();
		auto sdp = root["sdp"].asString();
		//auto capture_value = root["caputretype"];


		std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
		GBMEDIASERVER_LOG(LS_INFO) << "get session name:" << session_name << ", sdp:" << sdp;


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
		std::shared_ptr<RtcConsumer> consumer = std::dynamic_pointer_cast<RtcConsumer>(s->CreateConsumer(conn,
			session_name, "", ShareResourceType::kConsumerTypeRTC));

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
		//consumer->SetRtcRemoteAddress(conn->GetSocket()->GetRemoteAddress());
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
		s->AddConsumer(consumer);
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
		//PlayRtcUserPtr rtc_user = std::dynamic_pointer_cast<PlayRtcUser>(user);
		if (!consumer->ProcessOfferSdp(libmedia_transfer_protocol::librtc::kRtcSdpPlay, sdp))
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
		result["server"] = "GbMediaServer";
		result["type"] = "answer";
		result["sdp"] = std::move(answer_sdp);
		result["sessionid"] = consumer->RemoteUFrag() + ":" + consumer->LocalUFrag();

		auto content = result.toStyledString();
		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
			auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			res->SetStatusCode(200);
			res->AddHeader("server", "GbMediaServer");
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

		GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
		//�ɼ�����Ļ���
		//if (!capture_value.isNull() && capture_value.isInt())
		//{
		//	consumer->SetCapture(capture_value.asInt() > 0 ? true : false);
		//}
		//�������Ϊ stun����ʱ��Ҫ��֤�û��������� ������Ҫ�ֵ�ȫ����rtc����������ȥ
		RtcService::GetInstance().RegisterRtcInterface(consumer);
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();

	}
	
	void WebService::HandlerFlvConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
		std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "request:" << req->Path();
		std::string streamUrl = req->Path();
		auto pos = streamUrl.find_last_of('.');
		if (pos != std::string::npos)
		{
			streamUrl = "http://chensong.com" + streamUrl.substr(0, pos);
		}
		GBMEDIASERVER_LOG(LS_INFO) << "streamUrl:" << streamUrl;
		std::string session_name = string_utils::GetSessionNameFromUrl(streamUrl);
		GBMEDIASERVER_LOG(LS_INFO) << "flv  session name:" << session_name;
		//GBMEDIASERVER_LOG(LS_INFO) << "ext:" << ext;

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
		std::shared_ptr<Consumer> consumer = s->CreateConsumer(conn,
			session_name, "", ShareResourceType::kConsumerTypeFlv);

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
		GBMEDIASERVER_LOG(LS_INFO) << "flv   consumer : count : " << consumer.use_count();


		conn->SetContext(libmedia_transfer_protocol::libnetwork::kShareResourceContext, consumer);
		auto flv = std::make_shared<libmedia_transfer_protocol::libflv::FlvEncoder>(conn
#if 0
			, "gb28181_test.flv"
#endif // test 
			);
		conn->SetContext(libmedia_transfer_protocol::libnetwork::kFlvContext, flv);

		s->AddConsumer((consumer));
		GBMEDIASERVER_LOG(LS_INFO) << "flv   consumer : count : " << consumer.use_count();



	}
	void WebService::HandlerM3u8Consumer(libmedia_transfer_protocol::libnetwork::Connection * conn, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet, 
		std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "request:" << req->Path();
		std::string streamUrl = req->Path();
		auto pos = streamUrl.find_last_of('.');
		if (pos != std::string::npos)
		{
			streamUrl = "http://chensong.com" + streamUrl.substr(0, pos);
		}
		GBMEDIASERVER_LOG(LS_INFO) << "streamUrl:" << streamUrl;
		std::string session_name = string_utils::GetSessionNameFromUrl(streamUrl);
		GBMEDIASERVER_LOG(LS_INFO) << "m3u8  session name:" << session_name;
		//GBMEDIASERVER_LOG(LS_INFO) << "ext:" << ext;

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

		std::string paylist  = s->GetStream()->GetPlayList();
		if (!paylist.empty())
		{
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				res->AddHeader("server", "GbMediaServer");
				res->AddHeader("content-length", std::to_string(paylist.size()));
				res->AddHeader("content-type", "application/vnd.apple.mpegurl");

				res->SetStatusCode(200);
				res->SetBody(paylist);
				http_ctx->PostRequest(res);
			});
		}
		else
		{
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
				res->AddHeader("server", "GbMediaServer");
				http_ctx->PostRequest(res);
			});
		}
	}
	void WebService::HandlerTsConsumer(libmedia_transfer_protocol::libnetwork::Connection * conn, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req, 
		const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet, 
		std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	{
		GBMEDIASERVER_LOG(LS_INFO) << "request:" << req->Path();
		std::string streamUrl = req->Path();
		auto pos = streamUrl.find_last_of('.');
		if (pos != std::string::npos)
		{
			streamUrl = "http://chensong.com" + streamUrl.substr(0, pos);
		}
		GBMEDIASERVER_LOG(LS_INFO) << "streamUrl:" << streamUrl;
		std::string session_name = string_utils::GetSessionNameFromUrl(streamUrl);
		GBMEDIASERVER_LOG(LS_INFO) << "ts  session name:" << session_name;
		//GBMEDIASERVER_LOG(LS_INFO) << "ext:" << ext;

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
		std::string  filename;
		 
		{
			std::shared_ptr< libmedia_transfer_protocol::libhls::Fragment> frag  = s->GetStream()->GetFragement(filename);
			
			http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
				auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				res->AddHeader("server", "GbMediaServer");
				// TODO@chensong 2025-11-17
				res->AddHeader("content-length", std::to_string(frag->Size()));
				res->AddHeader("content-type", "video/MP2T");

				res->SetStatusCode(200);
				//res->SetBody(frag->Data());
				// TODO@chensong 2025-11-17
				//http_ctx->PostRequest(res->MakeHeaders(), frag->FragmentData());
			});
		}
	}
	void WebService::HandlerOpenRtpServer(libmedia_transfer_protocol::libnetwork::Connection* conn,
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

		std::string streamUrl = "gb28181://chensong.com/live/" + stream_id;
		std::string session_name = string_utils::GetSessionNameFromUrl(streamUrl);
		GBMEDIASERVER_LOG(LS_INFO) << "open rtp port:" << port << ", tcp_mode: " << tcp_mode << ", stream_id: " << session_name; // session name : " << session_name;


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
		auto producer = s->CreateProducer(session_name, "", ShareResourceType::kProducerTypeGB28181);

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

		rtp->SetContext(libmedia_transfer_protocol::libnetwork::kShareResourceContext, producer);


		Json::Value result;
		result["code"] = 0;
		result["tcpmode"] = tcp_mode;
		result["streamid"] = stream_id;
		result["port"] = tcp_port;
		++tcp_port;

		auto content = result.toStyledString();
		GBMEDIASERVER_LOG(LS_INFO) << " open rtp server info :" << content;

		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
			auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			res->SetStatusCode(200);
			res->AddHeader("server", "GbMediaServer");
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
	void WebService::HandlerCloseRtpServer(libmedia_transfer_protocol::libnetwork::Connection* conn,
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

		std::string streamurl = "gb28181://chensong.com/live/" + stream_id;
		std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
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
			res->AddHeader("server", "GbMediaServer");
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

	//void WebService::HandlerRtspProducer(libmedia_transfer_protocol::libnetwork::Connection* conn,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
	//	std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	//{
	//	GBMEDIASERVER_LOG(LS_INFO) << "RTSP producer request:" << packet->Data();
	//	
	//	// 解析请求参数
	//	std::string query = req->Query();
	//	std::string streamurl = req->GetQueryParam("streamurl");
	//	
	//	if (streamurl.empty())
	//	{
	//		GBMEDIASERVER_LOG(LS_WARNING) << "RTSP producer: missing streamurl parameter";
	//		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
	//			http_ctx->PostRequest(res);
	//			http_ctx->WriteComplete(conn);
	//		});
	//		return;
	//	}

	//	std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
	//	GBMEDIASERVER_LOG(LS_INFO) << "RTSP producer session name:" << session_name;

	//	auto s = GbMediaService::GetInstance().CreateSession(session_name);
	//	if (!s)
	//	{
	//		GBMEDIASERVER_LOG(LS_WARNING) << "cant create session name:" << session_name;
	//		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//			http_ctx->PostRequest(res);
	//			http_ctx->WriteComplete(conn);
	//		});
	//		return;
	//	}

	//	// 创建 RTSP Producer
	//	std::shared_ptr<RtspProducer> producer = std::dynamic_pointer_cast<RtspProducer>(
	//		s->CreateProducer(session_name, "", ShareResourceType::kProducerTypeRtsp));

	//	if (!producer)
	//	{
	//		GBMEDIASERVER_LOG(LS_WARNING) << "cant create RTSP producer session name:" << session_name;
	//		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//			http_ctx->PostRequest(res);
	//			http_ctx->WriteComplete(conn);
	//		});
	//		return;
	//	}

	//	s->SetProducer(producer);
	//	
	//	// 将 producer 关联到连接，用于接收 RTSP 数据
	//	conn->SetContext<ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext, producer);

	//	// 返回成功响应
	//	Json::Value result;
	//	result["code"] = 0;
	//	result["message"] = "RTSP producer created";
	//	result["session"] = session_name;

	//	auto content = result.toStyledString();
	//	http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//		auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
	//		res->SetStatusCode(200);
	//		res->AddHeader("server", "GbMediaServer");
	//		res->AddHeader("content-length", std::to_string(content.size()));
	//		res->AddHeader("content-type", "application/json");
	//		res->AddHeader("Access-Control-Allow-Origin", "*");
	//		res->SetBody(content);
	//		http_ctx->PostRequest(res);
	//		http_ctx->WriteComplete(conn);
	//	});

	//	GBMEDIASERVER_LOG(LS_INFO) << "RTSP producer created successfully, session:" << session_name;
	//}

	//void WebService::HandlerRtspConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::HttpRequest> req,
	//	const std::shared_ptr<libmedia_transfer_protocol::libhttp::Packet> packet,
	//	std::shared_ptr < libmedia_transfer_protocol::libhttp::HttpContext> http_ctx)
	//{
	//	GBMEDIASERVER_LOG(LS_INFO) << "RTSP consumer request:" << packet->Data();

	//	// 解析请求参数
	//	std::string streamurl = req->GetQueryParam("streamurl");
	//	
	//	if (streamurl.empty())
	//	{
	//		GBMEDIASERVER_LOG(LS_WARNING) << "RTSP consumer: missing streamurl parameter";
	//		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp400Response();
	//			http_ctx->PostRequest(res);
	//			http_ctx->WriteComplete(conn);
	//		});
	//		return;
	//	}

	//	std::string session_name = string_utils::GetSessionNameFromUrl(streamurl);
	//	GBMEDIASERVER_LOG(LS_INFO) << "RTSP consumer session name:" << session_name;

	//	auto s = GbMediaService::GetInstance().FindSession(session_name);
	//	if (!s)
	//	{
	//		GBMEDIASERVER_LOG(LS_WARNING) << "session not found:" << session_name;
	//		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//			http_ctx->PostRequest(res);
	//			http_ctx->WriteComplete(conn);
	//		});
	//		return;
	//	}

	//	// 创建 RTSP Consumer
	//	std::shared_ptr<RtspConsumer> consumer = std::dynamic_pointer_cast<RtspConsumer>(
	//		s->CreateConsumer(conn, session_name, "", ShareResourceType::kConsumerTypeRtsp));

	//	if (!consumer)
	//	{
	//		GBMEDIASERVER_LOG(LS_WARNING) << "cant create RTSP consumer session name:" << session_name;
	//		http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp404Response();
	//			http_ctx->PostRequest(res);
	//			http_ctx->WriteComplete(conn);
	//		});
	//		return;
	//	}

	//	s->AddConsumer(consumer);
	//	
	//	// 将 consumer 关联到连接
	//	conn->SetContext<ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext, consumer);

	//	// 返回成功响应
	//	Json::Value result;
	//	result["code"] = 0;
	//	result["message"] = "RTSP consumer created";
	//	result["session"] = session_name;

	//	auto content = result.toStyledString();
	//	http_server_->network_thread()->PostTask(RTC_FROM_HERE, [=]() {
	//		auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
	//		res->SetStatusCode(200);
	//		res->AddHeader("server", "GbMediaServer");
	//		res->AddHeader("content-length", std::to_string(content.size()));
	//		res->AddHeader("content-type", "application/json");
	//		res->AddHeader("Access-Control-Allow-Origin", "*");
	//		res->SetBody(content);
	//		http_ctx->PostRequest(res);
	//		http_ctx->WriteComplete(conn);
	//	});

	//	GBMEDIASERVER_LOG(LS_INFO) << "RTSP consumer created successfully, session:" << session_name;
	//}
}
