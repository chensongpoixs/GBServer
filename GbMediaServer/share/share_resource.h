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
#ifndef _C_RTC_SHARE_RESOURCE_H_
#define _C_RTC_SHARE_RESOURCE_H_

#include <string>
#include <cstdint>
#include <memory>   
#include <vector>  
#include <string>

#include <atomic>

#include "rtc_base/copy_on_write_buffer.h"

#include "libmedia_codec/encoded_image.h"
#include "rtc_base/socket_address.h"
namespace  gb_media_server
{

	enum  ShareResourceType
	{
	 
		kProducerTypePublishGB28181 = 0,
		//  
		kConsumerTypePlayerWebRTC,
		kShareResourceTypeUnknowed = 255,


	};
	class Stream;
	class Session;
	class ShareResource : public std::enable_shared_from_this<ShareResource>
	{
	public:
		explicit ShareResource(const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
		virtual ~ShareResource();

	public:
		const std::string & AppName() const;
		void SetAppName(const std::string & app_name);
		const std::string & StreamName() const;
		void SetStreamName(const std::string & stream);
		const std::string & Param() const;
		void SetParam(const std::string & param);




		//���ܲ�ͬЭ���ϲ㴦�� ʵ��
		virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}


		virtual ShareResourceType ShareResouceType() const { return kShareResourceTypeUnknowed; }
		 
		const rtc::SocketAddress &RemoteAddress() const
		{
			return remote_address_;
		}
		void  SetRemoteAddress(const rtc::SocketAddress & addr);

		std::shared_ptr<Session> GetSession() const
		{
			return session_;
		}
		std::shared_ptr < Stream> GetStream() const
		{
			return stream_;
		}
		std::shared_ptr < Stream> GetStream()
		{
			return stream_;
		}
	protected:
		std::shared_ptr < Stream> stream_;
		std::string     app_name_;
		std::string     stream_name_;
		std::string     param_; 
		rtc::SocketAddress   remote_address_;
		int64_t			start_timestamp_{ 0 }; // ��ʼʱ�� 
		 
		std::shared_ptr < Session> session_;
	};
}

#endif // 