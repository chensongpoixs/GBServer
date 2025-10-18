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
				   date:  2025-10-12



 ******************************************************************************/
#ifndef _C__SIP_SERVER_H_
#define _C__SIP_SERVER_H_

#include <string>
#include <atomic>
extern "C" {
#include <osip2/osip_mt.h>
#include <eXosip2/eXosip.h>
}
#include <unordered_map>
#include <memory>
#include <memory>
#include "db/DeviceDb.hpp"
#include "oatpp/network/Server.hpp"
#include "AppComponent.hpp"
#include "libp2p_peerconnection/connection_context.h"
namespace gbsip_server
{

	struct SipServerInfo
	{
		std::string ua;
		std::string nonce; //sip服务的随机数字
		std::string ip; // sip 
		uint16_t   port; // sip 
		std::string  sipServerId; //  sip服务的ID
		std::string  SipServerRealm; //sip服务域名
		std::string  SipSeverPass; // sip password 
		int32_t      SipTimeout;
		int32_t      SipExpiry; //sip
		
	};


	struct SipClient
	{
		std::string ip;
		uint16_t     port;
		std::string device;
		bool    isreg;
		uint16_t   rtp_port;
	};


	class SipServer
	{
	public:
		typedef  void (SipServer::*SipEventCallback)(eXosip_event_t * sip_event)    ;
	public:
		SipServer();
		~SipServer();

		static SipServer & GetInstance()
		{
			static SipServer   instance;
			//GBMEDIASERVER_LOG_F(LS_INFO) << "instance: " << &instance;
			return instance;
		}
	public:
		OATPP_COMPONENT(std::shared_ptr<DeviceDb>,  device_db_);
	public:
		bool init(const SipServerInfo & info);
		bool Start();


		void request_invite() { request_invite_ = true; };



		rtc::Thread* signaling_thread() { return context_->signaling_thread(); }
		const rtc::Thread* signaling_thread() const { return context_->signaling_thread(); }
		rtc::Thread* worker_thread() { return context_->worker_thread(); }
		const rtc::Thread* worker_thread() const { return context_->worker_thread(); }
		rtc::Thread* network_thread() { return context_->network_thread(); }
		const rtc::Thread* network_thread() const { return context_->network_thread(); }


		void LoopSip();

	public:

		void HandlerSipcallMessageNew(eXosip_event_t * sip_event);
		void HandlerSipCallClosed(eXosip_event_t * sip_event);
		void HandlerSipCallReleased(eXosip_event_t * sip_event);
		void HandlerSipCallInvite(eXosip_event_t * sip_event);
		void HandlerSipCallProceeding(eXosip_event_t * sip_event);
		void HandlerSipCallAnswered(eXosip_event_t * sip_event);
		void HandlerSipCallServerFailure(eXosip_event_t * sip_event);

		void HandlerSipMessageNew(eXosip_event_t * sip_event);
		void HandlerSipMessageAnswerd(eXosip_event_t * sip_event);
		void HandlerSipMessageRequestFailure(eXosip_event_t * sip_event);
		
		
		void HandlerSipInSubscriptionNew(eXosip_event_t * sip_event);
		 
	public:


		void  request_info(eXosip_event_t * sip_event);
		void  response_info(eXosip_event_t * sip_event);

		void  response_register(eXosip_event_t *sip_event);
		void   response_message_answer(eXosip_event_t * sip_event, int32_t code);


		int32_t   request_invite(const std::string&  device, const std::string& remote_ip, uint16_t remote_port, uint16_t rtp_port);
		void     response_message(eXosip_event_t * sip_event);
		void response_register_401unauthorized(eXosip_event_t * sip_event);
	private:
	 	webrtc::ScopedTaskSafety task_safety_;
		rtc::scoped_refptr<libp2p_peerconnection::ConnectionContext>	context_;
		std::atomic<bool>  stoped_;
		SipServerInfo    sip_server_info_;
		struct eXosip_t *sip_context_;

		
		
		std::unordered_map<int32_t, SipEventCallback>        sip_event_callback_map_;


		std::unordered_map<std::string, std::shared_ptr<SipClient>>        client_map_;
		bool request_invite_ = false;
		int32_t  delay_ = 10;
	};
}



#endif // _C__SIP_SERVER_H_

