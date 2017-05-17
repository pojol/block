#include "event.h"


void gsf::EventModule::execute()
{
	while (!cmd_list_.empty())
	{
		auto itr = cmd_list_.begin();
		auto type_id = std::get<0>(*itr);

		auto tItr = type_map_.find(type_id);
		if (tItr != type_map_.end()) {
			auto _cmd_id = std::get<1>(*itr);
			auto iItr = tItr->second.find(_cmd_id);
			if (iItr != tItr->second.end()) {
				iItr->second(std::get<2>(*itr), std::get<3>(*itr));
			}
		}

		cmd_list_.pop_front();
	}
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

void gsf::EventModule::add_cmd(uint32_t type_id, uint32_t event, gsf::Args args, CallbackFunc callback /* = nullptr */)
{
	/*
	if (event == eid::network::recv_remote_callback) {

		uint32_t _module_id = args.pop_uint32(0);	//预留在这里，如果是分布式则需要将这次注册同步到协调Server
		uint32_t _msg_id = args.pop_uint32(1);
		auto _func = args.pop_remote_callback(2);

		auto itr = remote_map_.find(_module_id);
		if (itr != remote_map_.end()) {
			itr->second.push_back(std::make_pair(_msg_id, _func));
		}
		else {
			std::vector<std::pair<uint32_t, RemoteFunc>> vec;
			vec.push_back(std::make_pair(_msg_id, _func));
			remote_map_.insert(std::make_pair(_module_id, vec));
		}

		return;
	}
	*/
	cmd_list_.push_back(std::make_tuple(type_id, event, args, callback));
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

void gsf::IEvent::dispatch(uint32_t target, uint32_t event, gsf::Args args, CallbackFunc callback /* = nullptr */)
{
	EventModule::get_ref().add_cmd(target, event, args, callback);
}

void gsf::IEvent::bind_clear(uint32_t module_id)
{
	EventModule::get_ref().rmv_event(module_id);
}
