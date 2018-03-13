
#include "msgBinder.h"
#include <assert.h>

void gsf::network::MsgBinder::regist(RemoteInfoPtr info_ptr)
{
#ifdef _DEBUG
	assert(remote_map_.find(info_ptr->msg_id_) == remote_map_.end());
#endif // DEBUG

	remote_map_.insert(std::make_pair(info_ptr->msg_id_, info_ptr));
}

void gsf::network::MsgBinder::unregist(uint32_t msg_id)
{
	auto _itr = remote_map_.find(msg_id);
	if (_itr != remote_map_.end()) {
		remote_map_.erase(_itr);
	}
}

gsf::CallbackFunc gsf::network::MsgBinder::get_func(uint32_t msg_id)
{
	auto _itr = remote_map_.find(msg_id);

	if (_itr != remote_map_.end()) {
		return _itr->second->func_;
	}

	return nullptr;
}

uint32_t gsf::network::MsgBinder::get_module(uint32_t msg_id)
{
	auto _itr = remote_map_.find(msg_id);

	if (_itr != remote_map_.end()) {
		return _itr->second->module_id_;
	}

	return 0;
}

