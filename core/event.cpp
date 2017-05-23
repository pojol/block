#include "event.h"


void gsf::EventModule::execute()
{

}

gsf::EventModule::EventModule()
	: Module("EventModule")
{

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

void gsf::EventModule::add_cmd(uint32_t type_id, uint32_t event, const gsf::Args &args, CallbackFunc callback /* = nullptr */)
{
	auto tItr = type_map_.find(type_id);
	if (tItr != type_map_.end()) {

		auto iItr = tItr->second.find(event);
		if (iItr != tItr->second.end()) {
			iItr->second(args, callback);
		}
	}
}

void gsf::EventModule::rmv_event(uint32_t module_id)
{
	//! 清理消息协议事件相关的绑定
	//auto itr = remote_map_.find(module_id);
	//if (itr != remote_map_.end()) {
	//	remote_map_.erase(itr);
	//}

	//! 清理listen相关的事件绑定
	auto tItr = type_map_.find(module_id);
	if (tItr != type_map_.end()) {
		type_map_.erase(tItr);
	}
}

gsf::IEvent::IEvent()
{
}

gsf::IEvent::~IEvent()
{

}

void gsf::IEvent::listen(Module *target, uint32_t event, EventFunc func)
{
	EventModule::get_ref().bind_event(target->get_module_id(), event, func);
}

void gsf::IEvent::listen(uint32_t self, uint32_t event, EventFunc func)
{
	EventModule::get_ref().bind_event(self, event, func);
}

void gsf::IEvent::dispatch(uint32_t target, uint32_t event, const Args &args, CallbackFunc callback /* = nullptr */)
{
	EventModule::get_ref().add_cmd(target, event, args, callback);
}

void gsf::IEvent::bind_clear(uint32_t module_id)
{
	EventModule::get_ref().rmv_event(module_id);
}
