



#include "server/connection.h"

namespace gb_media_server
{

	Connection::Connection(rtc::Socket * socket)
		:socket_(socket) {
	}

	 
	const rtc::SocketAddress &Connection::LocalAddr() const
	{
		return socket_->GetLocalAddress();
	}
	const rtc::SocketAddress &Connection::PeerAddr() const
	{
		return socket_->GetRemoteAddress();
	}
	void Connection::SetContext(int type, const std::shared_ptr<void> &context)
	{
		contexts_[type] = context;
	}
	void Connection::SetContext(int type, std::shared_ptr<void> &&context)
	{
		contexts_[type] = std::move(context);
	}
	void Connection::ClearContext(int type)
	{
		contexts_[type].reset();
	}
	void Connection::ClearContext()
	{
		contexts_.clear();
	}
	void Connection::SetActiveCallback(const std::function<void(const std::shared_ptr<Connection>&)> &cb)
	{
		//active_cb_ = cb;
	}
	void Connection::SetActiveCallback(std::function<void(const std::shared_ptr<Connection>&)> &&cb)
	{
		//active_cb_ = std::move(cb);
	}
	void Connection::Active()
	{
		if (!active_.load())
		{
			//loop_->RunInLoop([this]() {
			//	active_.store(true);
			//	if (active_cb_)
			//	{
			//		active_cb_(std::dynamic_pointer_cast<Connection>(shared_from_this()));
			//	}
			//});
		}
	}
	void Connection::Deactive()
	{
		active_.store(false);
	}

}