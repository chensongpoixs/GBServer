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
#include "sip_server.h"


#ifndef WIN32
 // Linux系统
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif // !WIN32


#include "rtc_base/logging.h"


#include <cstring>
namespace gbsip_server
{
	SipServer::SipServer()
		: stoped_(true)
		, sip_server_info_()
		, sip_context_(nullptr)
	{
#ifdef WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			 
			return;
		}
#endif // WIN32


		sip_event_callback_map_[EXOSIP_CALL_MESSAGE_NEW] = &SipServer::HandlerSipcallMessageNew;
		sip_event_callback_map_[EXOSIP_CALL_CLOSED] = &SipServer::HandlerSipCallClosed;
		sip_event_callback_map_[EXOSIP_CALL_RELEASED] = &SipServer::HandlerSipCallReleased;
		sip_event_callback_map_[EXOSIP_MESSAGE_NEW] = &SipServer::HandlerSipMessageNew;
		sip_event_callback_map_[EXOSIP_MESSAGE_ANSWERED] = &SipServer::HandlerSipMessageAnswerd;
		sip_event_callback_map_[EXOSIP_MESSAGE_REQUESTFAILURE] = &SipServer::HandlerSipMessageRequestFailure;
		sip_event_callback_map_[EXOSIP_CALL_INVITE] = &SipServer::HandlerSipCallInvite;
		sip_event_callback_map_[EXOSIP_CALL_PROCEEDING] = &SipServer::HandlerSipCallProceeding;
		sip_event_callback_map_[EXOSIP_CALL_ANSWERED] = &SipServer::HandlerSipCallAnswered;

		sip_event_callback_map_[EXOSIP_CALL_SERVERFAILURE] = &SipServer::HandlerSipCallServerFailure;
		sip_event_callback_map_[EXOSIP_IN_SUBSCRIPTION_NEW] = &SipServer::HandlerSipInSubscriptionNew;
		//sip_event_callback_map_[EXOSIP_CALL_ANSWERED] = &SipServer::HandlerSipCallAnswered;
	}
	SipServer::~SipServer()
	{
	}
	bool SipServer::init(const SipServerInfo & info)
	{
		sip_server_info_ = std::move(info);

		sip_context_ = eXosip_malloc();
		if (!sip_context_) {
			RTC_LOG(LS_WARNING) << "eXosip_malloc error";
			return -1;
		}
		if (eXosip_init(sip_context_)) {
			RTC_LOG(LS_WARNING) << "eXosip_init error";
			return -1;
		}
		if (eXosip_listen_addr(sip_context_, IPPROTO_UDP, nullptr, sip_server_info_.port, AF_INET, 0)) {
			RTC_LOG(LS_WARNING) << "eXosip_listen_addr error";
			return -1;
		}
		eXosip_set_user_agent(sip_context_, sip_server_info_.ua.c_str());
		if (eXosip_add_authentication_info(sip_context_, sip_server_info_.sipServerId.c_str(), 
			sip_server_info_.sipServerId.c_str(), sip_server_info_.SipSeverPass.c_str(), NULL,
			sip_server_info_.SipServerRealm.c_str())) {
			RTC_LOG(LS_WARNING) << "eXosip_add_authentication_info error";
			return -1;
		}
		stoped_ = false;
		return true;
		//return false;
	}
	bool SipServer::Start()
	{
		if (!sip_context_)
		{
			return false;
		}
		while (!stoped_)
		{
			eXosip_event_t *evtp = eXosip_event_wait(sip_context_, 0, 20);
			if (!evtp) {
				eXosip_automatic_action(sip_context_);
				osip_usleep(100000);
				continue;
			}
			 
			eXosip_automatic_action(sip_context_);
			//this->sip_event_handle(evtp);
			eXosip_event_free(evtp);
		}



		return false;
	}
	void SipServer::HandlerSipcallMessageNew(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipCallClosed(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipCallReleased(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipCallInvite(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipCallProceeding(eXosip_event_t * sip_event)
	{
	}
	void SipServer::HandlerSipCallAnswered(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipCallServerFailure(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipMessageNew(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipMessageAnswerd(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipMessageRequestFailure(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
	void SipServer::HandlerSipInSubscriptionNew(eXosip_event_t * sip_event)
	{
		RTC_LOG_F(LS_INFO) << "";
	}
}