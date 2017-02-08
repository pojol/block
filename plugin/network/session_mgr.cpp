#include "session_mgr.h"
#include "session.h"

#include "err.h"


gsf::network::SessionMgr::SessionMgr(uint32_t index)
{
	session_index_ = index;
	session_index_beg_ = index;
}

gsf::network::SessionMgr::~SessionMgr()
{

}

gsf::network::SessionPtr gsf::network::SessionMgr::make_session(::bufferevent *bev, int fd)
{
	if (session_index_ < session_index_beg_ + SESSION_MAX_CONNECT){
		session_index_++;
	}
	else {
		session_index_ = session_index_beg_;
	}

	auto _session_ptr = std::make_shared<Session>(session_index_, bev, fd);
	session_queue_.insert(std::make_pair(_session_ptr->get_id(), _session_ptr));

	return _session_ptr;
}


int gsf::network::SessionMgr::close(int session_id)
{
	return 0;
}

gsf::network::SessionPtr gsf::network::SessionMgr::find(int session_id)
{
	auto _session_itr = session_queue_.find(session_id);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second;
	}

	return nullptr;
}
