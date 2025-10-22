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
				   date:  2025-10-14



 ******************************************************************************/

#include "share/share_resource.h"
#include "rtc_base/time_utils.h" 
#include "server/session.h" 
#include "consumer/consumer.h"
#include "server/stream.h"
namespace  gb_media_server
{
	ShareResource::ShareResource(const std::shared_ptr<Stream>& stream, const std::shared_ptr<Session>& s)
		: stream_(stream), session_(s)
	{
		start_timestamp_ = rtc::TimeMillis();
	}
	ShareResource::~ShareResource()
	{
	}
	const std::string & ShareResource::AppName() const
	{
		// TODO: insert return statement here
		return app_name_;
	}
	void ShareResource::SetAppName(const std::string & app_name)
	{
		app_name_ = app_name;
	}
	const std::string & ShareResource::StreamName() const
	{
		// TODO: insert return statement here
		return stream_name_;
	}
	void ShareResource::SetStreamName(const std::string & stream)
	{
		stream_name_ = stream;
	}
	const std::string & ShareResource::Param() const
	{
		// TODO: insert return statement here
		return param_;
	}
	void ShareResource::SetParam(const std::string & param)
	{
		param_ = param;
	}
	void ShareResource::SetRemoteAddress(const rtc::SocketAddress & addr)
	{
		remote_address_ = addr;
	}
}

