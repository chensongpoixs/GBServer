﻿/*
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
				   date:  2025-09-17



 ******************************************************************************/

//#include "libmedia_transfer_protocol/libsip/udp_sip_server.h"
//#include "libmedia_transfer_protocol/libsip/sip_server.h"
#include <thread>
#include <chrono>
#include "sip_server.h"
 
//#pragma comment(lib, "mil.lib")


#include "AppComponent.hpp"

#include "controller/UserController.hpp"
#include "controller/DeviceController.hpp"
#include "controller/StaticController.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"
#include "oatpp-swagger/Controller.hpp"

#include "oatpp/network/Server.hpp"

#include <iostream>
#include "rtc_base/logging.h"
//std::shared_ptr<gbsip_server::SipServer> sip_server;
void run() {

	
	AppComponent components; // Create scope Environment components
	//std::thread([=]() {
	//	OATPP_COMPONENT(std::shared_ptr<UserDb>, m_database);
	//	while (true)
	//	{
	//		if (!m_database)
	//		{
	//			printf("--> not deviceDb !!!\n");
	//		}
	//		else
	//		{
	//			oatpp::UInt32  param1;
	//			oatpp::UInt32 param2 = 19;
	//			auto data=	m_database->getAllUsers(param1, 90);
	//			printf("---> devie OK !!!\n");
	//		}
	//
	//		std::this_thread::sleep_for(std::chrono::seconds(30));
	//	}
	//}).detach();
	/* Get router component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	oatpp::web::server::api::Endpoints docEndpoints;

	docEndpoints.append(router->addController(UserController::createShared())->getEndpoints());
//	oatpp::web::server::api::Endpoints DevicedocEndpoints;
	docEndpoints.append(router->addController(DeviceController::createShared())->getEndpoints());

	router->addController(oatpp::swagger::Controller::createShared(docEndpoints));
	//router->addController(oatpp::swagger::Controller::createShared(docEndpoints));
	router->addController(StaticController::createShared());
	
	/* Get connection handler component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	/* Create a CORSInterceptor instance */
	auto corsInterceptor = std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>();
	auto AllowOptionsGlobal_ = std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>();

	/* Add CORSInterceptor to the connection handler */
	auto httpConnectionHandler = std::static_pointer_cast<oatpp::web::server::HttpConnectionHandler>(connectionHandler);
	httpConnectionHandler->addRequestInterceptor(AllowOptionsGlobal_);
	httpConnectionHandler->addResponseInterceptor(corsInterceptor);
	
	/* Get connection provider component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);
	//connectionProvider->
	/* create server */
	oatpp::network::Server server(connectionProvider,
		connectionHandler);
	gbsip_server::SipServerInfo  SipServerInfo;
	SipServerInfo.ua = "GbSipServer";
	SipServerInfo.ip = "192.168.1.2";
	SipServerInfo.port = 15060;
	SipServerInfo.nonce = "4101050000";
	SipServerInfo.sipServerId = "41010500002000000001";
	SipServerInfo.SipServerRealm = "4101050000";
	SipServerInfo.SipSeverPass = "12345678";
	SipServerInfo.SipTimeout = 1800;
	SipServerInfo.SipExpiry = 3600;
	gbsip_server::SipServer::GetInstance().init(SipServerInfo);
	gbsip_server::SipServer::GetInstance().Start();
	//sip_server = std::make_shared<gbsip_server::SipServer >();
	
	//sip_server->init(SipServerInfo);
	//std::thread([&]() {
	//	sip_server->Start();
	//}).detach();
	///OATPP_LOGd("Server", "Running on port {}...", connectionProvider->getProperty("port").toString())
	SIPSERVER_LOG(LS_INFO) << "Web Server  run port:" << connectionProvider->getProperty("port").std_str() ;
		server.run();

	/* stop db connection pool */
	OATPP_COMPONENT(std::shared_ptr<oatpp::provider::Provider<oatpp::sqlite::Connection>>, dbConnectionProvider);
	dbConnectionProvider->stop();

}


int main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		return;
	}
#endif // WIN32
	oatpp::Environment::init();
	
	run();

	/* Print how many objects were created during app running, and what have left-probably leaked */
	/* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
	std::cout << "\nEnvironment:\n";
	std::cout << "objectsCount = " << oatpp::Environment::getObjectsCount() << "\n";
	std::cout << "objectsCreated = " << oatpp::Environment::getObjectsCreated() << "\n\n";

	oatpp::Environment::destroy();
	
	//WSADATA wsaData;
	//SOCKET ConnectSocket = INVALID_SOCKET;
	//WSAEVENT EventArray[1];
	//DWORD EventTotal = 0, Index;
	////LPSOCKET_INFORMATION SocketInfo;

	//// 初始化Winsock
	//if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
	//	printf("WSAStartup failed: %d\n", WSAGetLastError());
	//	return 1;
	//}
	//libmedia_transfer_protocol::libsip::SipServer  sip_server;


	//sip_server.network_thread()->PostTask(RTC_FROM_HERE, [&]() {
	//	sip_server.Start();
	//});

	//while (true)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}

	return 0;
}