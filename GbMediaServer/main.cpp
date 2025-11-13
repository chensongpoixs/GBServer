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
#include "server/web_service.h"
#include "libmedia_codec/audio_codec/adts_header.h"

	


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



static bool stoped = false;



static FILE* out_log_file_ptr = NULL;


static std::mutex g_log_lock;


static void RtcLogCallback(const char* message)
{
	std::lock_guard<std::mutex> lock(g_log_lock);
	if (!out_log_file_ptr)
	{
		std::string  log_file_name = "gbmedia_server" + std::to_string(::time(NULL)) + ".log";
		out_log_file_ptr = fopen(log_file_name.c_str(), "wb+");
	}
	if (out_log_file_ptr)
	{
		fprintf(out_log_file_ptr, "%s", message);
	}
	fprintf(stdout, "%s", message);
}

int main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		return 0;
	}
#endif // WIN32

	 rtc::SetRtcLogOutCallback(&RtcLogCallback);


	const char* config_file = "gbmedia_server.yaml";
	if (argc > 2)
	{
		config_file = argv[1];
	}
	
	  
	bool init = gb_media_server::GbMediaService::GetInstance().Init(config_file);
	if (init)
	{
		gb_media_server::GbMediaService::GetInstance().Start();

		while (!stoped)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	gb_media_server::GbMediaService::GetInstance().Destroy();
	 
	 

	return EXIT_SUCCESS;
}