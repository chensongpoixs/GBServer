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


	class SipServer
	{
	public:
		typedef  void (SipServer::*SipEventCallback)(eXosip_event_t * sip_event)    ;
	public:
		SipServer();
		~SipServer();

	public:
		bool init(const SipServerInfo & info);
		bool Start();




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
		 


	private:
		std::atomic<bool>  stoped_;
		SipServerInfo    sip_server_info_;
		struct eXosip_t *sip_context_;

		
		
		std::unordered_map<int32_t, SipEventCallback>        sip_event_callback_map_;
	};
}



#endif // _C__SIP_SERVER_H_

