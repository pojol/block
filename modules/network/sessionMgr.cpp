#include "sessionMgr.h"
#include "session.h"

#include <core/application.h>

block::network::SessionMgr::SessionMgr(block::Module *base)
	: basePtr_(base)
{
}

block::network::SessionMgr::~SessionMgr()
{

}

block::network::SessionPtr block::network::SessionMgr::makeSession(int fd, block::ModuleID module_id, ::bufferevent *bev)
{
	target_ = module_id;

	auto _session_ptr = std::make_shared<Session>(fd, module_id, this, bev);
	sessionQueue_.insert(std::make_pair(fd, _session_ptr));
	sessionQueueByModule_.insert(std::make_pair(module_id, _session_ptr));

	return _session_ptr;
}

block::network::SessionPtr block::network::SessionMgr::find(int fd)
{
	auto _session_itr = sessionQueue_.find(fd);
	if (_session_itr != sessionQueue_.end()){
		return _session_itr->second;
	}

	return nullptr;
}

block::network::SessionPtr block::network::SessionMgr::findByModule(block::ModuleID module_id)
{
	auto _session_ptr = sessionQueueByModule_.find(module_id);

	if (_session_ptr != sessionQueueByModule_.end()) {
		return _session_ptr->second;
	}

	return nullptr;
}

void block::network::SessionMgr::addClose(int fd)
{
	disconnectVec_.push_back(fd);
}

void block::network::SessionMgr::addConnect(int fd)
{
	connectVec_.push_back(fd);
}


void block::network::SessionMgr::addMessage(block::ArgsPtr args)
{
	messageQueue_.push(std::move(args));
}

void block::network::SessionMgr::exec()
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
			basePtr_->dispatch(target_, block::event::tcp_dis_connect, block::makeArgs(fd));
		}
	}

	disconnectVec_.clear();

	for (int fd : connectVec_)
	{
		basePtr_->dispatch(target_, block::event::tcp_new_connect, block::makeArgs(fd));
	}

	connectVec_.clear();

	while (!messageQueue_.empty()) {

		basePtr_->dispatch(target_, block::event::tcp_recv, std::move(messageQueue_.front()));

		messageQueue_.pop();
	}
}

int block::network::SessionMgr::curMaxConnect() const
{
	return 0;
}