#include "event.h"


void gsf::EventModule::execute()
{
	while (!cmd_list_.empty())
	{
		auto itr = cmd_list_.begin();

		auto tItr = type_map_.find(std::get<0>(*itr));
		if (tItr != type_map_.end()) {
			auto iItr = tItr->second.find(std::get<1>(*itr));
			if (iItr != tItr->second.end()) {
				iItr->second(std::get<2>(*itr), std::get<3>(*itr));
			}
		}

		cmd_list_.pop_front();
	}

}

gsf::EventModule::EventModule()
	: door_id_(1)
{

}

uint32_t gsf::EventModule::make_door_id()
{
	if (door_id_ == UINT32_MAX) {
		door_id_ = 1;
	}

	return door_id_++;
}

void gsf::EventModule::bind_event(uint32_t type_id, uint32_t event, EventFunc func)
{
	auto regf = [&](InnerMap &itr) {
		itr.insert(std::make_pair(event, func));
	};

	auto typeItr = type_map_.find(type_id);
	if (typeItr != type_map_.end()) {

		auto eventItr = typeItr->second.find(event);
		if (eventItr != typeItr->second.end()) {
			printf("repeated event!\n");
			return;
		}
		
		regf(typeItr->second);
	}
	else {
		InnerMap _map;
		regf(_map);

		type_map_.insert(std::make_pair(type_id, _map));
	}
}

void gsf::EventModule::add_cmd(uint32_t type_id, uint32_t event, gsf::Args args, EventHandlerPtr callback /*= nullptr*/)
{
	cmd_list_.push_back(std::make_tuple(type_id, event, args, callback));
}

gsf::Door::Door()
{
	// make id
	door_id_ = EventModule::get_ref().make_door_id();
}


void gsf::Door::listen(EventPair ep, EventFunc func)
{
	EventModule::get_ref().bind_event(ep.first, ep.second, func);
}

void gsf::Door::dispatch(EventPair ep, gsf::Args args, EventHandlerPtr callback /* = nullptr */)
{
	EventModule::get_ref().add_cmd(ep.first, ep.second, args, callback);
}