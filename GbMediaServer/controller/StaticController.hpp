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
				  date:  2025-09-17



******************************************************************************/
#ifndef CRUD_STATICCONTROLLER_HPP
#define CRUD_STATICCONTROLLER_HPP

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class StaticController : public oatpp::web::server::api::ApiController {
public:
  StaticController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers)
    : oatpp::web::server::api::ApiController(apiContentMappers)
  {}
public:

  static std::shared_ptr<StaticController> createShared(
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  ){
    return std::make_shared<StaticController>(apiContentMappers);
  }

  ENDPOINT("GET", "/", root) {
    const char* html =
      "<html lang='en'>"
      "  <head>"
      "    <meta charset=utf-8/>"
      "  </head>"
      "  <body>"
      "    <p>Hello CRUD example project!</p>"
      "    <a href='swagger/ui'>Checkout Swagger-UI page</a>"
      "  </body>"
      "</html>";
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif //CRUD_STATICCONTROLLER_HPP
