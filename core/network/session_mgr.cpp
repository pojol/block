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

int gsf::SessionMgr::send(int session_id, const uint8_t *data, uint32_t len)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second->send(data, len);
	}

	return 0;
}