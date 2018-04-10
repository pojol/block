#include "node.h"

#include <modules/network/acceptor.h>
#include <modules/network/connector.h>

#include <core/dynamic_module_factory.h>
#include <core/application.h>

#include <algorithm>
#include <iostream>
#include <fmt/format.h>

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
	logM_ = APP.getModule("LogModule");
	timerM_ = APP.getModule("TimerModule");

	assert(logM_ != gsf::ModuleNil);
	assert(timerM_ != gsf::ModuleNil);

	using namespace std::placeholders;
	mailboxPtr_->listen(eid::node::node_create, std::bind(&NodeModule::eCreateNode, this, _1, _2));
	mailboxPtr_->listen(eid::node::node_regist, std::bind(&NodeModule::eRegistNode, this, _1, _2));

	//listenRpc(std::bind(&NodeModule::eventRpc, this, _1, _2, _3, _4));

	mailboxPtr_->listen(eid::network::recv, [&](gsf::ModuleID target, gsf::ArgsPtr args) {

		auto _fd = args->pop_fd();
		auto _msgid = args->pop_msgid();
		auto _callbackid = args->pop_i64();

		auto _itr = callbackMap_.find(_callbackid);
		if (_itr != callbackMap_.end()) {
			/*
			auto _state = args->pop_bool();
			auto _progress = args->pop_i32();

			auto _off = sizeof(gsf::SessionID) + 1 + sizeof(int32_t) + 1 + sizeof(int64_t) + 1 + sizeof(bool) + 1 + sizeof(int32_t) + 1;
			auto _block = args->pop_block(_off, args->get_size());

			auto _info = _itr->second;

			auto _res = gsf::ArgsPool::get_ref().get();
			_res->push_block(_block.c_str(), args->get_size() - _off);
			//_info->callback(_res, _progress, _state);

			auto _titr = timerSet_.find(_info->timer_);
			assert(_titr != timerSet_.end());
			timerSet_.erase(_titr);

			if (_progress == -1) {
				callbackMap_.erase(_itr);
			}
			else { //reset timer
				mailboxPtr_->dispatch(timerM_, eid::timer::delay_milliseconds, gsf::makeArgs(delayTag_, rpcDelay_));
				timerSet_.insert(std::make_pair(delayTag_, _info));
			}
			*/
		}
		else {
			APP.WARN_LOG("Node", "can't find rpc callback", "{}\n", _callbackid);
		}

		return nullptr;
	});

	mailboxPtr_->listen(eid::timer::timer_arrive, [&](gsf::ModuleID target, gsf::ArgsPtr args) {
		auto _tag = args->pop_i32();

		auto _itr = timerSet_.find(_tag);
		if (_itr != timerSet_.end()) { // timeout

			auto _info = _itr->second;
			timerSet_.erase(_itr);

			auto _citr = callbackMap_.find(_info->id_);
			_info->callback(gsf::makeArgs(fmt::format("{} rpc callback timeout", _info->id_)), -1, false);
			callbackMap_.erase(_citr);
		}
	});
}

void gsf::modules::NodeModule::init()
{
	mailboxPtr_->pull();
}

void gsf::modules::NodeModule::execute()
{
	mailboxPtr_->pull();
}

void gsf::modules::NodeModule::shut()
{
	mailboxPtr_->pull();
}

void gsf::modules::NodeModule::eventRpc(gsf::EventID event, gsf::ModuleID moduleID, const gsf::ArgsPtr &args, gsf::RpcCallback callback)
{
	int64_t _callbackid = 0;

	gsf::ModuleID _connector_m = gsf::ModuleNil;
	auto itr = eventMap_.find(event);
	if (itr != eventMap_.end()) {
		_connector_m = itr->second.connecotr_m_;
	}
	else {
		if (callback) {
			std::string _errstr = "can't find event! you need to register first.";
			callback(gsf::makeArgs(_errstr), -1, false);
		}
		return;
	}

	if (callback) {
		_callbackid = APP.getUUID();

		auto _itr = callbackMap_.find(_callbackid);
		if (_itr != callbackMap_.end()) {
			callback(gsf::makeArgs("repeat rpc event!"), -1, false);
			return;
		}

		/* 这里的tag 处理要考虑下
		mailboxPtr_->dispatch(timerM_, eid::timer::delay_milliseconds, gsf::makeArgs(delayTag_, rpcDelay_));
		auto _callbackPtr = std::make_shared<CallbackInfo>();
		_callbackPtr->callback = callback;
		_callbackPtr->timer_ = delayTag_;
		_callbackPtr->event_ = event;
		_callbackPtr->id_ = _callbackid;

		callbackMap_.insert(std::make_pair(_callbackid, _callbackPtr));
		timerSet_.insert(std::make_pair(_callbackPtr->timer_, _callbackPtr));
		*/
	}

	if (args) {
	/*
		auto argsPtr = gsf::ArgsPool::get_ref().get();
		argsPtr->push(event);
		argsPtr->push(_callbackid);
		argsPtr->push_block(args->get_block(0, args->get_size()).c_str(), args->get_size());

		mailboxPtr_->dispatch(_connector_m, eid::network::send, std::move(argsPtr));
		*/
	}
	else {
		mailboxPtr_->dispatch(_connector_m, eid::network::send, gsf::makeArgs(event, _callbackid));
	}
}

void gsf::modules::NodeModule::registNode(gsf::ModuleID base, int event, const std::string &ip, int port)
{
	bool bRes = false;
	auto _moduleid = 0;

	for (auto nod : eventMap_)
	{
		if (nod.second.ip_ == ip && nod.second.port_ == port) {
			bRes = true;
			_moduleid = nod.second.connecotr_m_;
			break;
		}
	}

	auto itr = eventMap_.find(event);
	if (itr == eventMap_.end()) {

		gsf::ModuleID _connector_m = gsf::ModuleNil;

		if (bRes) {
			_connector_m = _moduleid;
		}
		else {
			_connector_m = APP.createDynamicModule("ConnectorModule");
		}

		auto _nod = NodeInfo();
		_nod.connecotr_m_ = _connector_m;
		_nod.event_ = event;
		_nod.ip_ = ip;
		_nod.port_ = port;
		_nod.base_ = base;
		eventMap_.insert(std::make_pair(event, _nod));
	}
}

void gsf::modules::NodeModule::eCreateNode(gsf::ModuleID target, gsf::ArgsPtr args)
{
	if (!service_) {
		id_ = args->pop_i32();
		targetM_ = args->pop_moduleid();
		type_ = args->pop_string();

		acceptorIP_ = args->pop_string();
		acceptorPort_ = args->pop_i32();

		auto _root_ip = args->pop_string();
		auto _root_port = args->pop_i32();

		auto _module_len = args->pop_i32();
		for (int i = 0; i < _module_len; ++i)
		{
			ModuleInfo info;
			info.moduleName = args->pop_string();
			info.moduleID = args->pop_moduleid();
			info.characteristic = args->pop_i32();
			modules_.push_back(info);
		}

		//! tmp
		registNode(getModuleID(), eid::distributed::coordinat_regist, _root_ip, _root_port);
		registNode(getModuleID(), eid::distributed::coordinat_unregit, _root_ip, _root_port);
		registNode(getModuleID(), eid::distributed::coordinat_adjust_weight, _root_ip, _root_port);
		registNode(getModuleID(), eid::distributed::coordinat_select, _root_ip, _root_port);
		//

		mailboxPtr_->listen(eid::network::new_connect, [&](gsf::ModuleID target, gsf::ArgsPtr args) {
			connectorFD_ = args->pop_fd();

			if (!service_) {
				APP.INFO_LOG("Node", "connect root succ!");
				service_ = true;

				auto _args = gsf::ArgsPool::get_ref().get();
				_args->push(type_);
				_args->push(id_);
				_args->push(acceptorIP_);
				_args->push(acceptorPort_);
				_args->push(int32_t(modules_.size()));
				for (auto &it : modules_)
				{
					_args->push(it.moduleName);
					_args->push(it.moduleID);
					_args->push(it.characteristic);
				}
				eventRpc(eid::distributed::coordinat_regist, getModuleID(), _args, [&](const gsf::ArgsPtr &args, int32_t progress, bool result) {
					if (result) {
						mailboxPtr_->dispatch(targetM_, eid::node::node_create_succ, nullptr);
					}
				});
			}
		});

		mailboxPtr_->listen(eid::base::module_init_succ, [&](gsf::ModuleID target, gsf::ArgsPtr args) {
			/*
			auto _t = gsf::ArgsPool::get_ref().get();
			_t->push_block(args->pop_block(0, args->get_size()).c_str(), args->get_size());
			auto _module_id = _t->pop_moduleid();
			
			for (auto nod : eventMap_)
			{
				if (nod.second.connecotr_m_ == _module_id) {
					mailboxPtr_->dispatch(_module_id, eid::network::make_connector, gsf::makeArgs(getModuleID(), nod.second.ip_, nod.second.port_));
					mailboxPtr_->dispatch(nod.second.base_, eid::node::node_regist_succ, nullptr);
					break;
				}
			}
			*/
		});

	}
}

void gsf::modules::NodeModule::eRegistNode(gsf::ModuleID target, gsf::ArgsPtr args)
{
	auto _base = args->pop_i32();
	auto _event = args->pop_i32();
	auto _ip = args->pop_string();
	auto _port = args->pop_i32();

	registNode(_base, _event, _ip, _port);
}
