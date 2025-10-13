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
#include "service/RtcService.hpp"
//#include "service/DeviceServer.hpp"


oatpp::Object<RtcDto> RtcService::createRtc(const oatpp::Object<RtcDto>& dto) {

	//auto dbResult = m_database->createDevice(dto);
	//OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

	//auto dievceId = oatpp::sqlite::Utils::getLastInsertRowId(dbResult->getConnection());

	return     RtcDto::createShared(); //getDeviceById(dievceId);

}
 