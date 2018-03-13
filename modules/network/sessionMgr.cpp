#include "sessionMgr.h"
#include "session.h"
#include "msgBinder.h"


gsf::network::SessionMgr::SessionMgr()
{
}

gsf::network::SessionMgr::~SessionMgr()
{

}

gsf::network::SessionPtr gsf::network::SessionMgr::makeSession(int fd, int module_id, MsgBinder *binder, ::bufferevent *bev)
{
	auto _session_ptr = std::make_shared<Session>(fd, module_id, binder, std::bind(&SessionMgr::setNeedClose, this, std::placeholders::_1), bev);
	sessionQueue_.insert(std::make_pair(fd, _session_ptr));
	sessionQueueByModule_.insert(std::make_pair(module_id, _session_ptr));

	return _session_ptr;
}

gsf::network::SessionPtr gsf::network::SessionMgr::find(int fd)
{
	auto _session_itr = sessionQueue_.find(fd);
	if (_session_itr != sessionQueue_.end()){
		return _session_itr->second;
	}

	return nullptr;
}

gsf::network::SessionPtr gsf::network::SessionMgr::findByModule(uint32_t module_id)
{
	auto _session_ptr = sessionQueueByModule_.find(module_id);

	if (_session_ptr != sessionQueueByModule_.end()) {
		return _session_ptr->second;
	}

	return nullptr;
}

void gsf::network::SessionMgr::setNeedClose(int fd)
{
	disconnectVec_.push_back(fd);
}

void gsf::network::SessionMgr::close()
{
	for (int fd : disconnectVec_)
	{
		auto _itr = sessionQueue_.find(fd);

		if (_itr != sessionQueue_.end()) {

			auto _mitr = sessionQueueByModule_.find(_itr->second->getModuleID());
			if (_mitr != sessionQueueByModule_.end()) {
				sessionQueueByModule_.erase(_mitr);
			}

			sessionQueue_.erase(_itr);
		}
	}

	disconnectVec_.clear();
}

int gsf::network::SessionMgr::curMaxConnect() const
{
	return 0;
}