#include "sessionMgr.h"
#include "session.h"

#include <core/application.h>

gsf::network::SessionMgr::SessionMgr(gsf::Module *base)
	: basePtr_(base)
{
}

gsf::network::SessionMgr::~SessionMgr()
{

}

gsf::network::SessionPtr gsf::network::SessionMgr::makeSession(int fd, int module_id, ::bufferevent *bev)
{
	target_ = module_id;

	auto _session_ptr = std::make_shared<Session>(fd, module_id, this, bev);
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

void gsf::network::SessionMgr::addClose(int fd)
{
	disconnectVec_.push_back(fd);
}

void gsf::network::SessionMgr::addConnect(int fd)
{
	connectVec_.push_back(fd);
}


void gsf::network::SessionMgr::addMessage(gsf::ArgsPtr args)
{
	messageQueue_.push(std::move(args));
}

void gsf::network::SessionMgr::exec()
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
			basePtr_->dispatch(target_, eid::network::dis_connect, gsf::makeArgs(fd));
		}
	}

	disconnectVec_.clear();

	for (int fd : connectVec_)
	{
		basePtr_->dispatch(target_, eid::network::new_connect, gsf::makeArgs(fd));
	}

	connectVec_.clear();

	while (!messageQueue_.empty()) {

		basePtr_->dispatch(target_, eid::network::recv, std::move(messageQueue_.front()));

		messageQueue_.pop();
	}
}

int gsf::network::SessionMgr::curMaxConnect() const
{
	return 0;
}