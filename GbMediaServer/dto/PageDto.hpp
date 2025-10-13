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
#ifndef CRUD_PAGEDTO_HPP
#define CRUD_PAGEDTO_HPP

#include "RtcDto.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

template<class T>
class PageDto : public oatpp::DTO {

  DTO_INIT(PageDto, DTO)

  DTO_FIELD(UInt32, offset);
  DTO_FIELD(UInt32, limit);
  DTO_FIELD(UInt32, count);
  DTO_FIELD(Vector<T>, items);

};

class RtcPageDto : public PageDto<oatpp::Object<RtcDto>> {

  DTO_INIT(RtcPageDto, PageDto<oatpp::Object<RtcDto>>)

};
 
#include OATPP_CODEGEN_END(DTO)

#endif //CRUD_PAGEDTO_HPP
