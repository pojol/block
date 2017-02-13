#include "mail.h"

void gsf::core::Mail::regist(uint32_t event, EventFunc func)
{
    map_.insert(std::make_pair(event, func));
}

void gsf::core::Mail::dispatch(uint32_t event, int args, EventHandlerPtr callback)
{
	auto itr = map_.find(event);
	if (itr != map_.end()) {
		itr->second(args, callback);
	}
}

void gsf::core::Mail::listen_result(Event *event, std::function<void(uint32_t event, uint32_t errcode)> callback)
{
	self_map_.insert(std::make_pair(1, callback));
}