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

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理WebRTC推流请求（Handler RTC Producer）
	 *  
	 *  该方法实现WebRTC推流的完整处理流程，包括JSON解析、SDP协商、会话创建和DTLS握手。
	 *  
	 *  处理流程：
	 *  1. 解析HTTP请求体中的JSON数据
	 *  2. 验证必需字段（streamurl和sdp）
	 *  3. 从streamurl提取会话名称
	 *  4. 创建或获取会话实例
	 *  5. 创建RTC生产者实例
	 *  6. 处理客户端的SDP Offer
	 *  7. 生成服务器的SDP Answer
	 *  8. 启动DTLS握手
	 *  9. 注册到RTC服务以处理STUN/TURN请求
	 *  10. 返回JSON响应给客户端
	 *  
	 *  JSON请求格式：
	 *  {
	 *      "streamurl": "app/stream",
	 *      "sdp": "v=0\r\no=...",
	 *      "type": "offer"
	 *  }
	 *  
	 *  JSON响应格式：
	 *  {
	 *      "code": 0,
	 *      "server": "WebServer",
	 *      "type": "answer",
	 *      "sdp": "v=0\r\no=...",
	 *      "sessionid": "remote_ufrag:local_ufrag"
	 *  }
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象
	 *  @param packet HTTP数据包对象，包含JSON请求体
	 *  @param http_ctx HTTP上下文对象，用于发送响应
	 *  @note 该方法在工作线程中执行
	 *  @note 如果JSON解析失败或缺少必需字段，返回错误响应
	 *  @note 生产者会被注册到RTC服务以处理ICE连接
	 *  @note sessionid由远程和本地的ICE ufrag组成，用于标识会话
	 */
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
				//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				//res->SetStatusCode(200);
				//res->AddHeader("server", "GbMediaServer");
				//res->AddHeader("content-length", std::to_string(content.size()));
				//res->AddHeader("content-type", "application/json");
				//res->AddHeader("Access-Control-Allow-Origin", "*");
				//res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
				//res->AddHeader("Allow", "POST, GET, OPTIONS");
				//res->AddHeader("Access-Control-Allow-Headers", "content-type");
				//res->AddHeader("Connection", "close");
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();
				
				res->AddHeader("content-type", "application/json");
				res->SetBody(content);
				res->AddHeader("content-length", std::to_string(content.size()));
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
			//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			//res->SetStatusCode(200);
			//res->AddHeader("server", "WebServer");
			//res->AddHeader("content-length", std::to_string(content.size()));
			//res->AddHeader("content-type", "application/json");
			//res->AddHeader("Access-Control-Allow-Origin", "*");
			//res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
			//res->AddHeader("Allow", "POST, GET, OPTIONS");
			//res->AddHeader("Access-Control-Allow-Headers", "content-type");
			//res->AddHeader("Connection", "close");
			//res->SetBody(content);
			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();

			res->AddHeader("content-type", "application/json");
			res->SetBody(content);
			res->AddHeader("content-length", std::to_string(content.size()));
			http_ctx->PostRequest(res);

			http_ctx->WriteComplete(conn);
			});

		GBMEDIASERVER_LOG(LS_INFO) << "rtc player producer : count : " << producer.use_count();
		 
		//锟斤拷锟斤拷锟斤拷锟轿?stun锟斤拷锟斤拷时锟斤拷要锟斤拷证锟矫伙拷锟斤拷锟斤拷锟斤拷锟斤拷 锟斤拷锟斤拷锟斤拷要锟街碉拷全锟斤拷锟斤拷rtc锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷去
		//RtcService::GetInstance().RegisterRtcInterface(producer);
		s->GetRtcService()->RegisterRtcInterface(producer);
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player producer : count : " << producer.use_count();

	}

	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理WebRTC拉流请求（Handler RTC Consumer）
	 *  
	 *  该方法实现WebRTC拉流的完整处理流程，包括JSON解析、SDP协商、消费者创建和DTLS握手。
	 *  
	 *  处理流程：
	 *  1. 解析HTTP请求体中的JSON数据
	 *  2. 验证必需字段（streamurl和sdp）
	 *  3. 从streamurl提取会话名称
	 *  4. 创建或获取会话实例
	 *  5. 创建RTC消费者实例
	 *  6. 将消费者添加到会话中
	 *  7. 处理客户端的SDP Offer
	 *  8. 生成服务器的SDP Answer
	 *  9. 启动DTLS握手
	 *  10. 注册到RTC服务以处理STUN/TURN请求
	 *  11. 返回JSON响应给客户端
	 *  
	 *  JSON请求格式：
	 *  {
	 *      "streamurl": "app/stream",
	 *      "sdp": "v=0\r\no=...",
	 *      "type": "offer"
	 *  }
	 *  
	 *  JSON响应格式：
	 *  {
	 *      "code": 0,
	 *      "server": "GbMediaServer",
	 *      "type": "answer",
	 *      "sdp": "v=0\r\no=...",
	 *      "sessionid": "remote_ufrag:local_ufrag"
	 *  }
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象
	 *  @param packet HTTP数据包对象，包含JSON请求体
	 *  @param http_ctx HTTP上下文对象，用于发送响应
	 *  @note 该方法在工作线程中执行
	 *  @note 消费者添加到会话后会自动接收媒体流
	 *  @note 消费者会被注册到RTC服务以处理ICE连接
	 *  @note 如果会话不存在或没有生产者，消费者将等待直到有流可用
	 */
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
				//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				//res->SetStatusCode(200);
				//res->AddHeader("server", "GbMediaServer");
				//res->AddHeader("content-length", std::to_string(content.size()));
				//res->AddHeader("content-type", "application/json");
				//res->AddHeader("Access-Control-Allow-Origin", "*");
				//res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
				//res->AddHeader("Allow", "POST, GET, OPTIONS");
				//res->AddHeader("Access-Control-Allow-Headers", "content-type");
				//res->AddHeader("Connection", "close");
				//res->SetBody(content);
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();

				res->AddHeader("content-type", "application/json");
				res->SetBody(content);
				res->AddHeader("content-length", std::to_string(content.size()));
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
			//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			//res->SetStatusCode(200);
			//res->AddHeader("server", "GbMediaServer");
			//res->AddHeader("content-length", std::to_string(content.size()));
			//res->AddHeader("content-type", "application/json");
			//res->AddHeader("Access-Control-Allow-Origin", "*");
			//res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
			//res->AddHeader("Allow", "POST, GET, OPTIONS");
			//res->AddHeader("Access-Control-Allow-Headers", "content-type");
			//res->AddHeader("Connection", "close");
			//res->SetBody(content);
			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();

			res->AddHeader("content-type", "application/json");
			res->SetBody(content);
			res->AddHeader("content-length", std::to_string(content.size()));
			http_ctx->PostRequest(res);

			http_ctx->WriteComplete(conn);
			});

		GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();
		//锟缴硷拷锟斤拷锟斤拷幕锟斤拷锟?		//if (!capture_value.isNull() && capture_value.isInt())
		//{
		//	consumer->SetCapture(capture_value.asInt() > 0 ? true : false);
		//}
		//锟斤拷锟斤拷锟斤拷锟轿?stun锟斤拷锟斤拷时锟斤拷要锟斤拷证锟矫伙拷锟斤拷锟斤拷锟斤拷锟斤拷 锟斤拷锟斤拷锟斤拷要锟街碉拷全锟斤拷锟斤拷rtc锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷去
		//RtcService::GetInstance().RegisterRtcInterface(consumer);
		  s->GetRtcService()->RegisterRtcInterface(consumer);
		GBMEDIASERVER_LOG(LS_INFO) << "rtc player consumer : count : " << consumer.use_count();

	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理HTTP-FLV拉流请求（Handler FLV Consumer）
	 *  
	 *  该方法实现HTTP-FLV流媒体的处理流程，创建FLV消费者和编码器，建立HTTP-FLV推流通道。
	 *  
	 *  处理流程：
	 *  1. 从请求路径中提取流URL
	 *  2. 移除.flv扩展名，构造完整的流URL
	 *  3. 从URL中提取会话名称
	 *  4. 创建或获取会话实例
	 *  5. 创建FLV消费者实例
	 *  6. 设置消费者的远程地址
	 *  7. 将消费者绑定到连接上下文
	 *  8. 创建FLV编码器并绑定到连接
	 *  9. 将消费者添加到会话中
	 *  10. 消费者开始接收媒体流并通过FLV编码器发送
	 *  
	 *  URL处理示例：
	 *  - 请求路径：/live/stream1.flv
	 *  - 处理后：http://chensong.com/live/stream1
	 *  - 会话名称：live/stream1
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象，包含FLV流的路径
	 *  @param packet HTTP数据包对象
	 *  @param http_ctx HTTP上下文对象
	 *  @note 该方法在工作线程中执行
	 *  @note FLV编码器会自动发送FLV头部和媒体数据
	 *  @note 消费者和编码器都绑定到连接上下文，连接断开时自动清理
	 *  @note 支持调试模式，可以将FLV流保存到文件（通过宏控制）
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理HLS M3U8播放列表请求（Handler M3U8 Consumer）
	 *  
	 *  该方法实现HLS（HTTP Live Streaming）播放列表的处理流程，返回M3U8文件内容。
	 *  
	 *  处理流程：
	 *  1. 从请求路径中提取流URL
	 *  2. 移除.m3u8扩展名，构造完整的流URL
	 *  3. 从URL中提取会话名称
	 *  4. 创建或获取会话实例
	 *  5. 从会话的Stream中获取播放列表内容
	 *  6. 如果播放列表存在，返回M3U8文件
	 *  7. 如果播放列表不存在，返回404错误
	 *  
	 *  URL处理示例：
	 *  - 请求路径：/live/stream1.m3u8
	 *  - 处理后：http://chensong.com/live/stream1
	 *  - 会话名称：live/stream1
	 *  
	 *  M3U8响应头：
	 *  - Content-Type: application/vnd.apple.mpegurl
	 *  - Server: GbMediaServer
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象，包含M3U8文件路径
	 *  @param packet HTTP数据包对象
	 *  @param http_ctx HTTP上下文对象，用于发送M3U8响应
	 *  @note 该方法在工作线程中执行
	 *  @note M3U8文件由Stream动态生成，包含最新的TS切片列表
	 *  @note 客户端会定期请求M3U8文件以获取最新切片
	 *  @note 响应在网络线程中发送，避免阻塞工作线程
	 */
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
				//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
				//res->AddHeader("server", "GbMediaServer");
				//res->AddHeader("content-length", std::to_string(paylist.size()));
				//res->AddHeader("content-type", "application/vnd.apple.mpegurl");
				//
				//res->SetStatusCode(200);
				//res->SetBody(paylist);
				auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();

				res->AddHeader("content-type", "application/json");
				res->SetBody(paylist);
				res->AddHeader("content-length", std::to_string(paylist.size()));
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理HLS TS切片请求（Handler TS Consumer）
	 *  
	 *  该方法实现HLS TS（Transport Stream）切片的处理流程，返回TS切片数据。
	 *  
	 *  处理流程：
	 *  1. 从请求路径中提取流URL
	 *  2. 移除.ts扩展名，构造完整的流URL
	 *  3. 从URL中提取会话名称
	 *  4. 创建或获取会话实例
	 *  5. 从会话的Stream中获取TS切片（Fragment）
	 *  6. 返回TS切片数据
	 *  
	 *  URL处理示例：
	 *  - 请求路径：/live/stream-0.ts
	 *  - 处理后：http://chensong.com/live/stream-0
	 *  - 会话名称：live/stream-0
	 *  
	 *  TS响应头：
	 *  - Content-Type: video/MP2T
	 *  - Server: GbMediaServer
	 *  - Content-Length: 切片大小
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象，包含TS切片文件路径
	 *  @param packet HTTP数据包对象
	 *  @param http_ctx HTTP上下文对象，用于发送TS切片响应
	 *  @note 该方法在工作线程中执行
	 *  @note TS切片数据由Stream管理，包含固定时长的音视频数据
	 *  @note 响应在网络线程中发送，避免阻塞工作线程
	 *  @note TODO标记表示部分功能待完善（PostRequest方法调用）
	 */
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
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理开启GB28181 RTP服务器请求（Handler Open RTP Server）
	 *  
	 *  该方法实现GB28181协议中开启RTP服务器的处理流程，用于接收GB28181设备推送的PS流。
	 *  
	 *  处理流程：
	 *  1. 解析HTTP请求体中的JSON数据
	 *  2. 提取端口号、TCP模式和流ID
	 *  3. 构造GB28181流URL并提取会话名称
	 *  4. 创建或获取会话实例
	 *  5. 开启TCP服务器监听指定端口
	 *  6. 创建GB28181生产者实例
	 *  7. 将生产者设置到会话中
	 *  8. 将生产者绑定到TCP服务器连接上下文
	 *  9. 返回JSON响应，包含分配的端口号
	 *  
	 *  JSON请求格式：
	 *  {
	 *      "port": 10000,
	 *      "tcpmode": 1,
	 *      "streamid": "34020000001320000001"
	 *  }
	 *  
	 *  JSON响应格式：
	 *  {
	 *      "code": 0,
	 *      "tcpmode": 1,
	 *      "streamid": "34020000001320000001",
	 *      "port": 20000
	 *  }
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象
	 *  @param packet HTTP数据包对象，包含JSON请求体
	 *  @param http_ctx HTTP上下文对象，用于发送响应
	 *  @note 该方法在工作线程中执行
	 *  @note TCP端口从20000开始自动递增分配
	 *  @note RTP服务器使用TCP协议接收PS流
	 *  @note 生产者会自动解析PS流并提取音视频数据
	 *  @note 如果会话或RTP服务器创建失败，返回404错误
	 */
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
			//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			//res->SetStatusCode(200);
			//res->AddHeader("server", "GbMediaServer");
			//res->AddHeader("content-length", std::to_string(content.size()));
			//res->AddHeader("content-type", "application/json; charset=UTF-8");
			//res->AddHeader("Access-Control-Allow-Origin", "*");
			//res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
			//res->AddHeader("Allow", "POST, GET, OPTIONS");
			//res->AddHeader("Access-Control-Allow-Headers", "content-type");
			//res->AddHeader("Connection", "close");
			//res->SetBody(content);

			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();
			res->AddHeader("content-type", "application/json; charset=UTF-8");
			res->SetBody(content);
			res->AddHeader("content-length", std::to_string(content.size()));
			http_ctx->PostRequest(res);

			http_ctx->WriteComplete(conn);
			//conn->Close();
			});

	}
	
	/***
	 *  @author chensong
	 *  @date 2025-10-18
	 *  @brief 处理关闭GB28181 RTP服务器请求（Handler Close RTP Server）
	 *  
	 *  该方法实现GB28181协议中关闭RTP服务器的处理流程，停止接收PS流并释放资源。
	 *  
	 *  处理流程：
	 *  1. 解析HTTP请求体中的JSON数据
	 *  2. 提取流ID
	 *  3. 构造GB28181流URL并提取会话名称
	 *  4. 关闭对应的会话
	 *  5. 会话关闭会自动：
	 *     - 停止RTP服务器
	 *     - 释放TCP端口
	 *     - 清理生产者资源
	 *     - 断开所有消费者
	 *  6. 返回JSON响应
	 *  
	 *  JSON请求格式：
	 *  {
	 *      "streamid": "34020000001320000001"
	 *  }
	 *  
	 *  JSON响应格式：
	 *  {
	 *      "code": 0,
	 *      "streamid": "34020000001320000001"
	 *  }
	 *  
	 *  @param conn 网络连接对象指针
	 *  @param req HTTP请求对象
	 *  @param packet HTTP数据包对象，包含JSON请求体
	 *  @param http_ctx HTTP上下文对象，用于发送响应
	 *  @note 该方法在工作线程中执行
	 *  @note 如果会话不存在，返回404错误
	 *  @note 关闭会话会自动清理所有相关资源
	 *  @note 响应在网络线程中发送，避免阻塞工作线程
	 */
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
			//auto res = std::make_shared<libmedia_transfer_protocol::libhttp::HttpRequest>(false);
			//res->SetStatusCode(200);
			//res->AddHeader("server", "GbMediaServer");
			//res->AddHeader("content-length", std::to_string(content.size()));
			//res->AddHeader("content-type", "application/json");
			//res->AddHeader("Access-Control-Allow-Origin", "*");
			//res->AddHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
			//res->AddHeader("Allow", "POST, GET, OPTIONS");
			//res->AddHeader("Access-Control-Allow-Headers", "content-type");
			//res->AddHeader("Connection", "close");
			//res->SetBody(content);
			//http_ctx->PostRequest(res);

			//http_ctx->WriteComplete(conn);
			auto res = libmedia_transfer_protocol::libhttp::HttpRequest::NewHttp200Response();
			res->AddHeader("content-type", "application/json; charset=UTF-8");
			res->SetBody(content);
			res->AddHeader("content-length", std::to_string(content.size()));
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
	//	// 瑙ｆ瀽璇锋眰鍙傛暟
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

	//	// 鍒涘缓 RTSP Producer
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
	//	// 灏?producer 鍏宠仈鍒拌繛鎺ワ紝鐢ㄤ簬鎺ユ敹 RTSP 鏁版嵁
	//	conn->SetContext<ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext, producer);

	//	// 杩斿洖鎴愬姛鍝嶅簲
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

	//	// 瑙ｆ瀽璇锋眰鍙傛暟
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

	//	// 鍒涘缓 RTSP Consumer
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
	//	// 灏?consumer 鍏宠仈鍒拌繛鎺?	//	conn->SetContext<ShareResource>(libmedia_transfer_protocol::libnetwork::kShareResourceContext, consumer);

	//	// 杩斿洖鎴愬姛鍝嶅簲
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
