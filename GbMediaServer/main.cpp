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
				   date:  2025-10-13



 ******************************************************************************/


#include <iostream>


#include <iostream>
#include "rtc_base/logging.h" 
#include "server/gb_media_service.h"
#include "libmedia_transfer_protocol/libsip/sip_server.h"
#include "server/rtc_service.h"



	


#if 0
#include <windows.h>

// 窗口过程函数声明
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// WinMain函数实现
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	WNDCLASSW wc = { 0 };

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		return 0;
	}
#endif // WIN32
	// 注册窗口类
	if (!RegisterClassW(&wc)) {
		return -1;
	}

	// 创建窗口
	CreateWindowW(L"myWindowClass", L"Sample Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL);
	gb_media_server::GbMediaService::GetInstance().Start("192.168.1.2", 20001);
	// 消息循环
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

// 窗口过程函数定义
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
	return 0;
}
#endif // 

int main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		return 0;
	}
#endif // WIN32
#if 0


	std::unique_ptr< libmedia_transfer_protocol::librtc::RtcServer> rtc_server_;
	std::unique_ptr<rtc::Thread>   network_thread_ = rtc::Thread::CreateWithSocketServer();
	network_thread_->SetName("network_thread", nullptr);

	network_thread_->Start();
	std::unique_ptr<rtc::BasicNetworkManager> default_network_manager_;
	std::unique_ptr< libice::BasicPacketSocketFactory>  default_socket_factory_;
	if (network_thread_->IsCurrent())
	{

		//rtc::InitRandom(rtc::Time32());

		// If network_monitor_factory_ is non-null, it will be used to create a
		// network monitor while on the network thread.
		default_network_manager_ = std::make_unique<rtc::BasicNetworkManager>(
			nullptr, network_thread_->socketserver());

		// TODO(bugs.webrtc.org/13145): Either require that a PacketSocketFactory
		// always is injected (with no need to construct this default factory), or get
		// the appropriate underlying SocketFactory without going through the
		// rtc::Thread::socketserver() accessor.
		default_socket_factory_ = std::make_unique<libice::BasicPacketSocketFactory>(
			network_thread_->socketserver());
	}
	else
	{

		network_thread_->PostTask(RTC_FROM_HERE, [&]() {
			//	RTC_DCHECK_RUN_ON(network_thread_);
				//rtc::InitRandom(rtc::Time32());

				// If network_monitor_factory_ is non-null, it will be used to create a
				// network monitor while on the network thread.
				//rtc::InitRandom(rtc::Time32());

			// If network_monitor_factory_ is non-null, it will be used to create a
			// network monitor while on the network thread.
			default_network_manager_ = std::make_unique<rtc::BasicNetworkManager>(
				nullptr, network_thread_->socketserver());

			// TODO(bugs.webrtc.org/13145): Either require that a PacketSocketFactory
			// always is injected (with no need to construct this default factory), or get
			// the appropriate underlying SocketFactory without going through the
			// rtc::Thread::socketserver() accessor.
			default_socket_factory_ = std::make_unique<libice::BasicPacketSocketFactory>(
				network_thread_->socketserver());

		});
	}
	rtc_server_ = std::make_unique<libmedia_transfer_protocol::librtc::RtcServer>(network_thread_.get());

	rtc_server_->SignalStunPacket.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnStun);
	rtc_server_->SignalDtlsPacket.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnDtls);
	rtc_server_->SignalRtpPacket.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnRtp);
	rtc_server_->SignalRtcpPacket.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnRtcp);



	rtc_server_->SignalStunPacketBuffer.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnStun);
	rtc_server_->SignalDtlsPacketBuffer.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnDtls);
	rtc_server_->SignalRtpPacketBuffer.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnRtp);
	rtc_server_->SignalRtcpPacketBuffer.connect(&gb_media_server::RtcService::GetInstance(), &gb_media_server::RtcService::OnRtcp);
	rtc_server_->Start("192.168.1.2", 20001);

	//libmedia_transfer_protocol::libsip::SipServer sip_server_;
	//sip_server_.Start();

	//gb_media_server::GbMediaService::GetInstance().Start("192.168.1.2", 20001);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//	gb_media_server::GbMediaService::GetInstance().network_thread()->Run();
			//gb_media_server::GbMediaService::GetInstance().worker_thread()->Run();
	}

#endif // 

	gb_media_server::RtcService::GetInstance().StartWebServer("192.168.1.2", 8001);
	gb_media_server::GbMediaService::GetInstance().Start("192.168.1.2", 9001);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	//oatpp::Environment::init();
	//
	//run();
	//
	///* Print how many objects were created during app running, and what have left-probably leaked */
	///* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
	//std::cout << "\nEnvironment:\n";
	//std::cout << "objectsCount = " << oatpp::Environment::getObjectsCount() << "\n";
	//std::cout << "objectsCreated = " << oatpp::Environment::getObjectsCreated() << "\n\n";
	//
	//oatpp::Environment::destroy();

	return EXIT_SUCCESS;
}