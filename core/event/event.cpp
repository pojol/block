#include "event.h"


void gsf::core::EventModule::execute()
{

}

void gsf::core::EventModule::add_event(uint32_t event, EventFunc func)
{
	map_.insert(std::make_pair(event, func));
}

void gsf::core::EventModule::add_cmd(uint32_t event, gsf::stream::OStream args, EventHandlerPtr callback /*= nullptr*/)
{

}

gsf::core::Door::Door()
{
	// make id
	door_id_ = 1;
}

void gsf::core::Door::listen(Door *self, EventFunc func)
{
	EventModule::get_ref().add_event(self->get_id(), func);
}

void gsf::core::Door::dispatch(uint32_t event, gsf::stream::OStream args, EventHandlerPtr callback /*= nullptr*/)
{
	EventModule::get_ref().add_cmd(event, args, callback);
}

void gsf::core::Door::listen(uint32_t event, EventFunc func)
{
	EventModule::get_ref().add_event(event, func);
}
