

/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-11-09



 ******************************************************************************/

#ifndef __GB_MEDIA_SERVER_LOG_H_
#define __GB_MEDIA_SERVER_LOG_H_
#include "rtc_base/logging.h"
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // GbMediaServer mobule
#define  GBMEDIASERVER_LOG(sev)  RTC_LOG(sev)  << "[GbMediaServer]"
#define  GBMEDIASERVER_LOG_F(sev)  RTC_LOG_F(sev) << "[GbMediaServer]"
#define  GBMEDIASERVER_LOG_T_F(sev)  RTC_LOG_T_F(sev) << "[GbMediaServer]"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GB_SIP_SERVER_LOG_H_