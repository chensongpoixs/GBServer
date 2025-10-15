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
#ifndef _C_RTC_API_Controller_hpp
#define _C_RTC_API_Controller_hpp
 
#include "service/RtcApiService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"
#include "dto/RtcApiDto.hpp"
 
#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * Device REST controller.
 */
class RtcApiController : public oatpp::web::server::api::ApiController {
public:
	RtcApiController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
    : oatpp::web::server::api::ApiController(apiContentMappers)
  {}
private:
  RtcApiService m_rtcApiService; // Create rtc service.
public:

  static std::shared_ptr<RtcApiController> createShared(
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  ){
    return std::make_shared<RtcApiController>(apiContentMappers);
  }
  
  ENDPOINT_INFO(createRtcApi) {
    info->summary = "Create new RtcApi";

    info->addConsumes<Object<RtcApiDto>>("application/json");

    info->addResponse<Object<RtcApiDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "RtcApi", createRtcApi,
           BODY_DTO(Object<RtcApiDto>, rtcApiDto))
  {
	//  m_defaultAuthorizationHandler->getRealm();

    return createDtoResponse(Status::CODE_200, m_rtcApiService.createRtcApi(rtcApiDto));
  }
  
  
 

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif /* UserController_hpp */
