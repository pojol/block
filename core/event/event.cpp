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
}

void gsf::core::EventModule::add_event(uint32_t event, EventFunc func)
{
	map_.insert(std::make_pair(event, func));
}

void gsf::core::EventModule::add_cmd(uint32_t event, gsf::stream::OStream args, EventHandlerPtr callback /*= nullptr*/)
{
	list_.push_back(std::make_tuple(event, args, callback));
}

gsf::core::Door::Door()
{
	// make id
	door_id_ = 1;
}

void gsf::core::Door::listen(Door *self, EventFunc func)
{
	EventModule::get_ref().add_event(self->get_door_id(), func);
}

void gsf::core::Door::dispatch(uint32_t event, gsf::stream::OStream args, EventHandlerPtr callback /*= nullptr*/)
{
	EventModule::get_ref().add_cmd(event, args, callback);
}

void gsf::core::Door::listen(uint32_t event, EventFunc func)
{
	EventModule::get_ref().add_event(event, func);
}
