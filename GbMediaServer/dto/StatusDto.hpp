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
#ifndef CRUD_STATUSDTO_HPP
#define CRUD_STATUSDTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class StatusDto : public oatpp::DTO {

  DTO_INIT(StatusDto, DTO)

  DTO_FIELD_INFO(status) {
    info->description = "Short status text";
  }
  DTO_FIELD(String, status);

  DTO_FIELD_INFO(code) {
    info->description = "Status code";
  }
  DTO_FIELD(Int32, code);

  DTO_FIELD_INFO(message) {
    info->description = "Verbose message";
  }
  DTO_FIELD(String, message);

};

#include OATPP_CODEGEN_END(DTO)

#endif //CRUD_STATUSDTO_HPP
