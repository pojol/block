#include "node.h"

#include <modules/network/acceptor.h>
#include <modules/network/connector.h>

#include <core/dynamic_module_factory.h>

#include <algorithm>
#include <iostream>


namespace gsf
{
	namespace network
	{
		REGISTER_CLASS(AcceptorModule)
		REGISTER_CLASS(ConnectorModule)
	}
}

gsf::modules::NodeModule::NodeModule()
	: Module("NodeModule")
{

}

gsf::modules::NodeModule::~NodeModule()
{

}

void gsf::modules::NodeModule::before_init()
{
	log_m_ = dispatch(eid::base::app_id, eid::get_module, gsf::make_args("LogModule"))->pop_moduleid();
	timer_m_ = dispatch(eid::base::app_id, eid::get_module, gsf::make_args("TimerModule"))->pop_moduleid();

	assert(log_m_ != gsf::ModuleNil);
	assert(timer_m_ != gsf::ModuleNil);

	using namespace std::placeholders;
	listen(this, eid::node::node_create, std::bind(&NodeModule::event_create_node, this, _1));
	listen(this, eid::node::node_regist, std::bind(&NodeModule::event_regist_node, this, _1));

	rpc_listen(std::bind(&NodeModule::event_rpc, this, _1, _2, _3));
}

void gsf::modules::NodeModule::init()
{
	listen(this, eid::network::recv, [&](const gsf::ArgsPtr &args) {
	
		auto _fd = args->pop_fd();
		auto _msgid = args->pop_msgid();

		auto _itr = callback_map_.find(_msgid);
		if (_itr != callback_map_.end()) {
			
			auto _info = _itr->second;
			_info->callback(args, true);

			callback_map_.erase(_itr);

			auto _titr = timer_set_.find(_info->timer_);
			assert(_titr != timer_set_.end());
			timer_set_.erase(_titr);
		}

		return nullptr;
	});

	listen(this, eid::timer::timer_arrive, [&](const gsf::ArgsPtr &args) {
		auto _tid = args->pop_ui64();

		auto _itr = timer_set_.find(_tid);
		if (_itr != timer_set_.end()) { // timeout

			auto _info = _itr->second;

			timer_set_.erase(_itr);

			auto _citr = callback_map_.find(_info->event_);
			_info->callback(gsf::make_args("timeout"), false);
			callback_map_.erase(_citr);
		}

		return nullptr;
	});
}

void gsf::modules::NodeModule::execute()
{

}

void gsf::modules::NodeModule::shut()
{

}

void gsf::modules::NodeModule::event_rpc(int event, const gsf::ArgsPtr &args, gsf::RpcCallback callback)
{
	auto _itr = callback_map_.find(event);
	if (_itr != callback_map_.end()) {
		dispatch(log_m_, eid::log::print, gsf::log_warring("NodeModule", "repeat rpc event!"));
		return;
	}

	gsf::ModuleID _connector_m = gsf::ModuleNil;
	auto itr = event_map_.find(event);
	if (itr != event_map_.end()) {
		_connector_m = itr->second.connecotr_m_;
	}
	else {
		if (callback) {
			std::string _errstr = "can't find event! you need to register first.";
			callback(gsf::make_args(_errstr), false);
		}
		return;
	}

	if (callback) {
		uint64_t _tid = dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(get_module_id(), rpc_delay_))->pop_timerid();

		auto _callbackPtr = std::make_shared<CallbackInfo>();
		_callbackPtr->callback = callback;
		_callbackPtr->timer_ = _tid;
		_callbackPtr->event_ = event;

		callback_map_.insert(std::make_pair(event, _callbackPtr));
		timer_set_.insert(std::make_pair(_tid, _callbackPtr));
	}

	if (args) {
		auto argsPtr = gsf::ArgsPool::get_ref().get();
		argsPtr->push(event);
		argsPtr->push_block(args->pop_block(0, args->get_pos()).c_str(), args->get_pos());

		dispatch(_connector_m, eid::network::send, std::move(argsPtr));
	}
	else {
		dispatch(_connector_m, eid::network::send, gsf::make_args(event));
	}
}

void gsf::modules::NodeModule::regist_node(gsf::ModuleID base, int event, const std::string &ip, int port)
{
	bool bRes = false;
	auto _moduleid = 0;

	for (auto nod : event_map_)
	{
		if (nod.second.ip_ == ip && nod.second.port_ == port) {
			bRes = true;
			_moduleid = nod.second.connecotr_m_;
			break;
		}
	}

	auto itr = event_map_.find(event);
	if (itr == event_map_.end()) {

		gsf::ModuleID _connector_m = gsf::ModuleNil;

		if (bRes) {
			_connector_m = _moduleid;
		}
		else {
			_connector_m = dispatch(eid::base::app_id, eid::base::new_dynamic_module, gsf::make_args("ConnectorModule"))->pop_moduleid();
		}

		auto _nod = NodeInfo();
		_nod.connecotr_m_ = _connector_m;
		_nod.event_ = event;
		_nod.ip_ = ip;
		_nod.port_ = port;
		_nod.base_ = base;
		event_map_.insert(std::make_pair(event, _nod));
	}
}

gsf::ArgsPtr gsf::modules::NodeModule::event_create_node(const gsf::ArgsPtr &args)
{
	if (!service_) {
		id_ = args->pop_i32();
		target_m_ = args->pop_moduleid();
		type_ = args->pop_string();

		acceptor_ip_ = args->pop_string();
		acceptor_port_ = args->pop_i32();

		root_ip_ = args->pop_string();
		root_port_ = args->pop_i32();

		auto _module_len = args->pop_i32();
		for (int i = 0; i < _module_len; ++i)
		{
			ModuleInfo info;
			info.moduleName = args->pop_string();
			info.moduleID = args->pop_moduleid();
			info.characteristic = args->pop_i32();
			modules_.push_back(info);
		}

		for (int i = eid::distributed::rpc_begin; i < eid::distributed::rpc_end; ++i)
		{
			regist_node(get_module_id(), i, root_ip_, root_port_);
		}

		listen(this, eid::network::new_connect, [&](const gsf::ArgsPtr &args) {
			connector_fd_ = args->pop_fd();

			if (!service_) {
				dispatch(log_m_, eid::log::print, gsf::log_info("NodeModule", "distributed nod connect 2 root!"));
				service_ = true;

				auto _args = gsf::ArgsPool::get_ref().get();
				_args->push(type_);
				_args->push(id_);
				_args->push(acceptor_ip_);
				_args->push(acceptor_port_);
				_args->push(int32_t(modules_.size()));
				for (auto &it : modules_)
				{
					_args->push(it.moduleName);
					_args->push(it.moduleID);
					_args->push(it.characteristic);
				}
				event_rpc(eid::distributed::coordinat_regist, _args, [&](const gsf::ArgsPtr &args, bool result) {
					if (result) {
						dispatch(target_m_, eid::node::node_create_succ, nullptr);
					}
				});
			}
			
			return nullptr;
		});

		listen(this, eid::base::module_init_succ, [&](const gsf::ArgsPtr &args) {

			auto _t = gsf::ArgsPool::get_ref().get();
			_t->push_block(args->pop_block(0, args->get_pos()).c_str(), args->get_pos());
			auto _module_id = _t->pop_moduleid();
			
			for (auto nod : event_map_)
			{
				if (nod.second.connecotr_m_ == _module_id) {
					dispatch(_module_id, eid::network::make_connector, gsf::make_args(get_module_id(), nod.second.ip_, nod.second.port_));
					dispatch(nod.second.base_, eid::node::node_regist_succ, nullptr);
					break;
				}
			}

			return nullptr;
		});

		return nullptr;
	}

	return nullptr;
}

gsf::ArgsPtr gsf::modules::NodeModule::event_regist_node(const gsf::ArgsPtr &args)
{
	auto _base = args->pop_i32();
	auto _event = args->pop_i32();
	auto _ip = args->pop_string();
	auto _port = args->pop_i32();

	regist_node(_base, _event, _ip, _port);

	return nullptr;
}