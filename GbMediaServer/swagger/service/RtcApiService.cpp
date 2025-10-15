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
#include "service/RtcApiService.hpp"
//#include "service/DeviceServer.hpp"
#include "user/play_rtc_user.h"
//#include "user/play_rtc.h"
#include "server/rtc_service.h"
oatpp::Object<RtcApiDto> RtcApiService::createRtcApi(const oatpp::Object<RtcApiDto>& dto) {

	//auto dbResult = m_database->createDevice(dto);
	//OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
	//gb_media_server::PlayRtcUser play_rtc;
	//play_rtc.ProcessOfferSdp(dto->sdp);
	//std::string answer = play_rtc.BuildAnswerSdp();
	////auto dievceId = oatpp::sqlite::Utils::getLastInsertRowId(dbResult->getConnection());
	//auto data = RtcApiDto::createShared();
	//data->sdp = answer;
	//data->type = "answer";
	return     gb_media_server::RtcService::GetInstance().CreateOfferAnswer(dto); //getDeviceById(dievceId);

}
 