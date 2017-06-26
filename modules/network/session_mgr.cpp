#include "session_mgr.h"
#include "session.h"
#include "msg_binder.h"


gsf::network::SessionMgr::SessionMgr()
{
}

gsf::network::SessionMgr::~SessionMgr()
{

}

gsf::network::SessionPtr gsf::network::SessionMgr::make_session(int fd, int module_id, MsgBinder *binder)
{
	auto _session_ptr = std::make_shared<Session>(fd, module_id, binder, std::bind(&SessionMgr::set_need_close, this, std::placeholders::_1));
	session_queue_.insert(std::make_pair(fd, _session_ptr));
	session_queue_bymodule_.insert(std::make_pair(module_id, _session_ptr));

	return _session_ptr;
}

gsf::network::SessionPtr gsf::network::SessionMgr::find(int fd)
{
	auto _session_itr = session_queue_.find(fd);
	if (_session_itr != session_queue_.end()){
		return _session_itr->second;
	}

	return nullptr;
}

gsf::network::SessionPtr gsf::network::SessionMgr::find_byModule(uint32_t module_id)
{
	auto _session_ptr = session_queue_bymodule_.find(module_id);

	if (_session_ptr != session_queue_bymodule_.end()) {
		return _session_ptr->second;
	}

	return nullptr;
}

void gsf::network::SessionMgr::set_need_close(int fd)
{
	disconnect_vec_.push_back(fd);
}

void gsf::network::SessionMgr::close()
{
	for (int fd : disconnect_vec_)
	{
		auto _itr = session_queue_.find(fd);

		if (_itr != session_queue_.end()) {

			auto _mitr = session_queue_bymodule_.find(_itr->second->get_module_id());
			if (_mitr != session_queue_bymodule_.end()) {
				session_queue_bymodule_.erase(_mitr);
			}

			session_queue_.erase(_itr);
		}
	}

	disconnect_vec_.clear();
}

int gsf::network::SessionMgr::cur_max_connet() const
{
	return 0;
}