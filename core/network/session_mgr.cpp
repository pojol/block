#include "session_mgr.h"
#include "session.h"

#include "err.h"

gsf::SessionMgr* gsf::SessionMgr::instance_ = nullptr;

gsf::SessionMgr& gsf::SessionMgr::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = new gsf::SessionMgr();
	}
	return *instance_;
}

static uint32_t session_index = 0;

gsf::SessionPtr gsf::SessionMgr::make_session(::bufferevent *bev, int fd)
{
	session_index++;

	auto _session_ptr = std::make_shared<Session>(session_index, bev, fd);
	session_queue_.insert(std::make_pair(_session_ptr->get_id(), _session_ptr));

	return _session_ptr;
}


int gsf::SessionMgr::open(int session_id, SessionHandler *session_handler, SessionCloseHandler *close_handler)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second->open(session_handler, close_handler);
	}
	else {
		return SESSION_NOT_FIND;
	}
}

int gsf::SessionMgr::close(int session_id)
{
	return 0;
}

int gsf::SessionMgr::write(int session_id, const char *data, uint32_t len)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		_session_itr->second->write(data, len);
	}

	return 0;
}

void gsf::SessionMgr::write_impl()
{
	//! 这个地方待优化 active_list 只有会话层需要发送数据的时候才被调用，不是遍历hash_map去调用。
	for (auto itr : session_queue_)
	{
		itr.second->write_impl();
	}
}

gsf::SessionPtr gsf::SessionMgr::find(int session_id)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second;
	}

	return nullptr;
}
