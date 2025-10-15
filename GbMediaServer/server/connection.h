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
#ifndef _C_CONNECTION_H____
#define _C_CONNECTION_H____

 
#include "user/user.h"
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <memory>
#include "absl/types/optional.h"
#include "rtc_base/system/rtc_export.h"
#include "rtc_base/physical_socket_server.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
#ifdef WIN32
#include "rtc_base/win32_socket_server.h"
#include <vcruntime.h>
#endif




#include <functional>

namespace  gb_media_server
{











	enum
	{
		kNormalContext = 0,
		kRtmpContext,
		kHttpContext,
		kUserContext,
		kFlvContext,
		kGb28181Context,
	};
	//struct BufferNode
	//{
	//	BufferNode(void *buf, size_t s)
	//		:addr(buf), size(s)
	//	{}
	//	void *addr{ nullptr };
	//	size_t size{ 0 };
	//};
	//using BufferNodePtr = std::shared_ptr<BufferNode>;
	//class Connection;
	//using ContextPtr = std::shared_ptr<void>;
	//class Connection;
	//using ConnectionPtr = std::shared_ptr<Connection>;
	//using ActiveCallback = std::function<void(const ConnectionPtr&)>;

	//typedef  std::shared_ptr<void>    ContextPtr;
	//typedef std::function<void(const ConnectionPtr&)>
	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		Connection( rtc::Socket* socket);
		virtual ~Connection() = default;

		//void SetLocalAddr(const InetAddress &local);
		//void SetPeerAddr(const InetAddress &peer);
		const rtc::SocketAddress &LocalAddr() const;
		const rtc::SocketAddress &PeerAddr() const;

		void SetContext(int type, const std::shared_ptr<void> &context);
		void SetContext(int type, std::shared_ptr<void> &&context);
		template <typename T> std::shared_ptr<T> GetContext(int type) const
		{
			auto iter = contexts_.find(type);
			if (iter != contexts_.end())
			{
				return std::static_pointer_cast<T>(iter->second);
			}
			return std::shared_ptr<T>();
		}
		void ClearContext(int type);
		void ClearContext();
		void SetActiveCallback(const std::function<void(const std::shared_ptr<Connection>&)> &cb);
		void SetActiveCallback(std::function<void(const std::shared_ptr<Connection>&)> &&cb);
		void Active();
		void Deactive();
		virtual void ForceClose() {};
	private:
		std::unordered_map<int, std::shared_ptr<void>> contexts_;
	//	ActiveCallback active_cb_;
		std::atomic<bool> active_{ false };
	protected:
		rtc::Socket *        socket_;
	};
}

#endif // 
