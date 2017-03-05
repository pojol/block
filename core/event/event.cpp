#include "event.h"


void gsf::core::EventModule::execute()
{
	while (!list_.empty())
	{
		auto itr = list_.begin();

		auto fitr = map_.find(std::get<0>(*itr));
		if (fitr != map_.end()) {
			fitr->second(std::get<1>(*itr), std::get<2>(*itr));
		}

		list_.pop_front();
	}

	while (!callback_list_.empty())
	{
		auto itr = callback_list_.begin();

		auto fitr = callback_map_.find(std::get<0>(*itr));
		if (fitr != callback_map_.end()) {
			fitr->second(std::get<1>(*itr));
		}

		callback_list_.pop_front();
	}
}

void gsf::core::EventModule::add_event(uint32_t event, EventFunc func)
{
	map_.insert(std::make_pair(event, func));
}

void gsf::core::EventModule::add_event(uint32_t event, std::function<void(gsf::Args)> func)
{
	callback_map_.insert(std::make_pair(event, func));
}

void gsf::core::EventModule::add_cmd(uint32_t door, gsf::Args args, EventHandlerPtr callback /*= nullptr*/)
{
	list_.push_back(std::make_tuple(door, args, callback));
}

void gsf::core::EventModule::add_cmd(uint32_t door, uint32_t sub_event, gsf::Args args)
{
	callback_list_.push_back(std::make_tuple(door + sub_event, args));
}

gsf::core::EventModule::EventModule()
	: door_id_(1)
{

}

uint32_t gsf::core::EventModule::make_door_id()
{
	if (door_id_ == UINT32_MAX) {
		door_id_ = 1;
	}

	return door_id_++;
}

gsf::core::Door::Door()
{
	// make id
	door_id_ = EventModule::get_ref().make_door_id();
}

void gsf::core::Door::dispatch(uint32_t door, gsf::Args args, EventHandlerPtr callback /*= nullptr*/)
{
	EventModule::get_ref().add_cmd(door, args, callback);
}

void gsf::core::Door::dispatch(uint32_t door, uint32_t sub_event, gsf::Args args)
{
	EventModule::get_ref().add_cmd(door, sub_event, args);
}

void gsf::core::Door::listen(uint32_t door, EventFunc func)
{
	EventModule::get_ref().add_event(door, func);
}

void gsf::core::Door::listen_callback(uint32_t sub_event, std::function<void(gsf::Args)> func)
{
	EventModule::get_ref().add_event(get_door_id() + sub_event, func);
}
