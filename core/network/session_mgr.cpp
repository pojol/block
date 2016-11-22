#include "session_mgr.h"
#include "session.h"

#include "err.h"

gsf::network::SessionMgr* gsf::network::SessionMgr::instance_ = nullptr;

gsf::network::SessionMgr& gsf::network::SessionMgr::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = new gsf::network::SessionMgr();
	}
	return *instance_;
}

static uint32_t session_index = 0;

gsf::network::SessionPtr gsf::network::SessionMgr::make_session(::bufferevent *bev, int fd)
{
	session_index++;

	auto _session_ptr = std::make_shared<Session>(session_index, bev, fd);
	session_queue_.insert(std::make_pair(_session_ptr->get_id(), _session_ptr));

	return _session_ptr;
}


int gsf::network::SessionMgr::open(int session_id, SessionHandler *session_handler, SessionCloseHandler *close_handler)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second->open(session_handler, close_handler);
	}
	else {
		return SESSION_NOT_FIND;
	}
}

int gsf::network::SessionMgr::close(int session_id)
{
	return 0;
}

int gsf::network::SessionMgr::write(int session_id, const char *data, uint32_t len)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		if (_session_itr->second->write(data, len) == 0){
			out_active_vec_.push_back(session_id);
		}
	}

	return 0;
}

void gsf::network::SessionMgr::write_impl()
{
	for (int session_id : out_active_vec_)
	{
		auto _session_itr = session_queue_.find(session_id);
		if (_session_itr != session_queue_.end()){
			_session_itr->second->write_impl();
		}
	}

	out_active_vec_.clear();
}

gsf::network::SessionPtr gsf::network::SessionMgr::find(int session_id)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second;
	}

	return nullptr;
}
