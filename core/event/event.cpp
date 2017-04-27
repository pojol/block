#include "event.h"
#include <sol.hpp>

void gsf::EventModule::execute()
{
	while (!cmd_list_.empty())
	{
		auto itr = cmd_list_.begin();

		auto tItr = type_map_.find(std::get<0>(*itr));
		if (tItr != type_map_.end()) {
			auto _cmd_id = std::get<1>(*itr);
			auto iItr = tItr->second.find(_cmd_id);
			if (iItr != tItr->second.end()) {
				iItr->second(std::get<2>(*itr), std::get<3>(*itr));
			}
		}

		cmd_list_.pop_front();
	}

	while (!remote_callback_list_.empty())
	{
		auto itr = remote_callback_list_.begin();

		auto _module_id = std::get<0>(*itr);
		auto _msg_id = std::get<1>(*itr);
		auto fItr = remote_map_.find(_module_id);
		if (fItr != remote_map_.end()) {

			BlockPtr _blockptr = std::get<3>(*itr);
			std::string _str(_blockptr->buf_, _blockptr->size_);

			auto lbItr = find_msg(fItr->second.begin(), fItr->second.end(), _msg_id);
			if (lbItr != fItr->second.end()) {
				try {
					lbItr->second(std::get<2>(*itr), _msg_id, _str);
				}
				catch (sol::error e) {
					std::cout << e.what() << std::endl;
				}
			}
		}

		remote_callback_list_.pop_front();
	}

	while (!remote_event_list_.empty())
	{
		auto itr = remote_event_list_.begin();

		uint32_t _eid = std::get<0>(*itr);
		auto fitr = remote_event_map_.find(_eid);
		if (fitr != remote_event_map_.end()){
			fitr->second(std::get<1>(*itr), std::get<2>(*itr), std::get<3>(*itr));
		}

		remote_event_list_.pop_front();
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

void gsf::EventModule::bind_remote_event(uint32_t type_id, RemoteEventFunc func)
{
	auto itr = remote_event_map_.find(type_id);
	if (itr != remote_event_map_.end()){
		return;
	}

	remote_event_map_.insert(std::make_pair(type_id, func));
}

void gsf::EventModule::add_cmd(uint32_t type_id, uint32_t event, gsf::Args args, CallbackFunc callback /* = nullptr */)
{
	if (event == eid::network::bind_remote_callback) {

		uint32_t _module_id = args.pop_uint32(0);	//预留在这里，如果是分布式则需要将这次注册同步到协调Server
		uint32_t _msg_id = args.pop_uint32(1);
		auto _func = args.pop_remote_callback(2);

		auto itr = remote_map_.find(_module_id);
		if (itr != remote_map_.end()) {
			itr->second.push_back(std::make_pair(_msg_id, _func));

			typedef std::vector<std::pair<uint32_t, RemoteFunc>> RNode;
			std::sort(itr->second.begin(), itr->second.end(), [&](RNode::value_type l, RNode::value_type r) {
				return (l.first < r.first);
			});
		}
		else {
			std::vector<std::pair<uint32_t, RemoteFunc>> vec;
			vec.push_back(std::make_pair(_msg_id, _func));
			remote_map_.insert(std::make_pair(_module_id, vec));
		}

		return;
	}

	cmd_list_.push_back(std::make_tuple(type_id, event, args, callback));
}

void gsf::EventModule::add_remote_callback(uint32_t module_id, uint32_t msg_id, uint32_t fd, BlockPtr blockptr)
{
	remote_callback_list_.push_back(std::make_tuple(module_id, msg_id, fd, blockptr));
}

void gsf::EventModule::rmv_event(uint32_t module_id)
{
	//! 清理消息协议事件相关的绑定
	auto itr = remote_map_.find(module_id);
	if (itr != remote_map_.end()) {
		remote_map_.erase(itr);
	}

	//! 清理listen相关的事件绑定
	auto tItr = type_map_.find(module_id);
	if (tItr != type_map_.end()) {
		type_map_.erase(tItr);
	}
}

gsf::EventModule::RNode::iterator gsf::EventModule::find_msg(RNode::iterator beg, RNode::iterator end, uint32_t msg)
{
	RNode::iterator it;
	std::iterator_traits<RNode::iterator>::difference_type count, step;
	count = std::distance(beg, end);
	while (count > 0)
	{
		it = beg; step = count / 2; std::advance(it, step);
		if (it->first < msg) {                 // or: if (comp(*it,val)), for version (2)
			beg = ++it;
			count -= step + 1;
		}
		else count = step;
	}
	return beg;
}

void gsf::EventModule::add_remote_cmd(uint32_t type_id, std::vector<uint32_t> fd_list, uint32_t msg_id, BlockPtr blockptr)
{
	remote_event_list_.push_back(std::make_tuple(type_id, fd_list, msg_id, blockptr));
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

void gsf::IEvent::remote_callback(uint32_t module_id, uint32_t fd, uint32_t msg_id, BlockPtr blockptr)
{
	EventModule::get_ref().add_remote_callback(module_id, msg_id, fd, blockptr);
}

void gsf::IEvent::bind_clear(uint32_t module_id)
{
	EventModule::get_ref().rmv_event(module_id);
}

void gsf::IEvent::listen_remote(Module *target, RemoteEventFunc func)
{
	EventModule::get_ref().bind_remote_event(target->get_module_id(), func);
}

void gsf::IEvent::dispatch_remote(uint32_t target, uint32_t fd, uint32_t msg_id, BlockPtr blockptr)
{
	std::vector<uint32_t> fd_list;
	fd_list.push_back(fd);
	EventModule::get_ref().add_remote_cmd(target, fd_list, msg_id, blockptr);
}

void gsf::IEvent::dispatch_remote(uint32_t target, uint32_t fd, uint32_t msg_id, const std::string &str)
{
	std::vector<uint32_t> fd_list;
	fd_list.push_back(fd);

	auto _msg = std::make_shared<gsf::Block>(fd, msg_id, str.length());
	memcpy(_msg->buf_ + _msg->pos_, str.c_str(), str.length());

	EventModule::get_ref().add_remote_cmd(target, fd_list, msg_id, _msg);
}
