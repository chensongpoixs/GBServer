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
				   date:  2025-10-14



 ******************************************************************************/
#ifndef _C_GB_MEDIA_SERVICE_H_
#define _C_GB_MEDIA_SERVICE_H_

#include "server/session.h"
#include "libp2p_peerconnection/connection_context.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"
#include "libmedia_transfer_protocol/libnetwork/tcp_server.h"
#include "rtc_base/logging.h"

namespace  gb_media_server
{


	//class Session;
	//using SessionPtr = std::shared_ptr<Session>;
	class GbMediaService : public sigslot::has_slots<>
	{
	public:
		explicit GbMediaService()  ;
		virtual ~GbMediaService( );



		static GbMediaService & GetInstance()
		{
			static GbMediaService   instance;
			//GBMEDIASERVER_LOG_F(LS_INFO) << "instance: "  << &instance;
			return instance;
		}

	public:
		std::shared_ptr < Session> CreateSession(const std::string &session_name, bool split=true);
		std::shared_ptr < Session> FindSession(const std::string &session_name );
		bool CloseSession(const std::string &session_name);
		//void OnTimer(const TaskPtr &t);


		libmedia_transfer_protocol::libnetwork::TcpServer*     OpenTcpServer(const std::string & stream_id, uint16_t port);




		void OnNewConnection(libmedia_transfer_protocol::libnetwork::Connection* conn);
		void OnDestory(libmedia_transfer_protocol::libnetwork::Connection* conn);
		void OnRecv(libmedia_transfer_protocol::libnetwork::Connection* conn, const rtc::CopyOnWriteBuffer& data);
		void OnSent(libmedia_transfer_protocol::libnetwork::Connection* conn);

	
		bool Init();
		void Start(const char * ip, uint16_t port);
		void Stop();
		void Destroy();

		uint16_t   RtpPort() const { return rtc_port_; }
		const std::string & RtpWanIp() const{ return rtc_wan_ip_; }
#if 1

		rtc::Thread* signaling_thread() { return context_->signaling_thread(); }
		const rtc::Thread* signaling_thread() const { return context_->signaling_thread(); }
		rtc::Thread* worker_thread() { return context_->worker_thread(); }
		const rtc::Thread* worker_thread() const { return context_->worker_thread(); }
		rtc::Thread* network_thread() { return context_->network_thread(); }
		const rtc::Thread* network_thread() const { return context_->network_thread(); }
#else 
		rtc::Thread* signaling_thread() { return worker_thread_.get(); }
		const rtc::Thread* signaling_thread() const { return worker_thread_.get(); }
		rtc::Thread* worker_thread() { return worker_thread_.get(); }
		const rtc::Thread* worker_thread() const { return worker_thread_.get(); }
		rtc::Thread* network_thread() { return network_thread_.get(); }
		const rtc::Thread* network_thread() const { return network_thread_.get(); }
#endif 

		 libmedia_transfer_protocol::librtc::RtcServer* GetRtcServer()const
		{
			return rtc_server_.get();
		}
	private:
		//EventLoopThreadPool * pool_{ nullptr };
		//std::vector<TcpServer*> servers_;
		rtc::scoped_refptr<libp2p_peerconnection::ConnectionContext>	context_;
		std::mutex lock_;
		std::unordered_map<std::string, std::shared_ptr < Session>> sessions_;

		std::unique_ptr<libmedia_transfer_protocol::librtc::RtcServer>   rtc_server_;



		std::unordered_map<std::string, std::unique_ptr<libmedia_transfer_protocol::libnetwork::TcpServer> > rtp_server_;// rtp map 
	//	std::unique_ptr<rtc::Thread>								network_thread_;
	//	std::unique_ptr<rtc::Thread>                               worker_thread_;
		//std::shared_ptr<WebrtcServer>  webrtc_server_;
		std::string										rtc_wan_ip_ = "192.168.1.2";
		uint16_t										rtc_port_ = 10009;
	};
}


#endif // #ifndef _C_API_TRANSPORT_STUN_H_
//#define _C_API_TRANSPORT_STUN_H_