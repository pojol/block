#include "node.h"

#include <core/application.h>

#include <algorithm>
#include <iostream>
#include <fmt/format.h>
#include <utils/timer.hpp>


block::modules::NodeModule::NodeModule()
	: Module("NodeModule")
{

}

block::modules::NodeModule::~NodeModule()
{

}

void block::modules::NodeModule::before_init()
{
	using namespace std::placeholders;
	listen(block::event::node_init, std::bind(&NodeModule::eNodeInit, this, _1, _2));
	
	//listen(block::event::node_regist, std::bind(&NodeModule::eRegistNode, this, _1, _2));

	//listenRpc(std::bind(&NodeModule::eventRpc, this, _1, _2, _3, _4));

	listen(block::event::tcp_recv, [&](block::ModuleID target, block::ArgsPtr args) {

		auto _fd = args->pop_fd();
		auto _msgid = args->pop_msgid();
		auto _callbackid = args->pop_i64();

		auto _itr = callbackMap_.find(_callbackid);
		if (_itr != callbackMap_.end()) {
			/*
			auto _state = args->pop_bool();
			auto _progress = args->pop_i32();

			auto _off = sizeof(block::SessionID) + 1 + sizeof(int32_t) + 1 + sizeof(int64_t) + 1 + sizeof(bool) + 1 + sizeof(int32_t) + 1;
			auto _block = args->pop_block(_off, args->get_size());

			auto _info = _itr->second;

			auto _res = block::ArgsPool::get_ref().get();
			_res->push_block(_block.c_str(), args->get_size() - _off);
			//_info->callback(_res, _progress, _state);

			auto _titr = timerSet_.find(_info->timer_);
			assert(_titr != timerSet_.end());
			timerSet_.erase(_titr);

			if (_progress == -1) {
				callbackMap_.erase(_itr);
			}
			else { //reset timer
				mailboxPtr_->dispatch(timerM_, eid::timer::delay_milliseconds, block::makeArgs(delayTag_, rpcDelay_));
				timerSet_.insert(std::make_pair(delayTag_, _info));
			}
			*/	
		}
		else {
			WARN_FMTLOG("node can't find rpc callback  callbackID:{}\n", _callbackid);
		}

		return nullptr;
	});

	/*
	listen(eid::timer::timer_arrive, [&](block::ModuleID target, block::ArgsPtr args) {
		auto _tag = args->pop_i32();

		auto _itr = timerSet_.find(_tag);
		if (_itr != timerSet_.end()) { // timeout

			auto _info = _itr->second;
			timerSet_.erase(_itr);

			auto _citr = callbackMap_.find(_info->id_);
			_info->callback(block::makeArgs(fmt::format("{} rpc callback timeout", _info->id_)), -1, false);
			callbackMap_.erase(_citr);
		}
	});
	*/
}

void block::modules::NodeModule::init()
{
}

void block::modules::NodeModule::execute()
{
}

void block::modules::NodeModule::shut()
{
}

void block::modules::NodeModule::eventRpc(block::EventID event, block::ModuleID moduleID, const block::ArgsPtr &args, block::RpcCallback callback)
{
	int64_t _callbackid = 0;

	block::ModuleID _connector_m = block::ModuleNil;
	auto itr = eventMap_.find(event);
	if (itr != eventMap_.end()) {
		_connector_m = itr->second.connecotr_m_;
	}
	else {
		if (callback) {
			std::string _errstr = "can't find event! you need to register first.";
			callback(block::makeArgs(_errstr), -1, false);
		}
		return;
	}

	if (callback) {
		_callbackid = APP.getUUID();

		auto _itr = callbackMap_.find(_callbackid);
		if (_itr != callbackMap_.end()) {
			callback(block::makeArgs("repeat rpc event!"), -1, false);
			return;
		}

		/* 这里的tag 处理要考虑下
		mailboxPtr_->dispatch(timerM_, eid::timer::delay_milliseconds, block::makeArgs(delayTag_, rpcDelay_));
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
		auto argsPtr = block::ArgsPool::get_ref().get();
		argsPtr->push(event);
		argsPtr->push(_callbackid);
		argsPtr->push_block(args->get_block(0, args->get_size()).c_str(), args->get_size());

		mailboxPtr_->dispatch(_connector_m, eid::network::send, std::move(argsPtr));
		*/
	}
	else {
		dispatch(_connector_m, block::event::tcp_send, block::makeArgs(event, _callbackid));
	}
}

void block::modules::NodeModule::registNode(block::ModuleID base, int event, const std::string &ip, int port)
{
	bool bRes = false;
	block::ModuleID _moduleid = 0;

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

		block::ModuleID _connector_m = block::ModuleNil;

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

void block::modules::NodeModule::eNodeInit(block::ModuleID target, block::ArgsPtr args)
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
		registNode(getModuleID(), block::event::coordinat_regist, _root_ip, _root_port);
		registNode(getModuleID(), block::event::coordinat_unregit, _root_ip, _root_port);
		registNode(getModuleID(), block::event::coordinat_adjust_weight, _root_ip, _root_port);
		registNode(getModuleID(), block::event::coordinat_select, _root_ip, _root_port);
		//

		listen(block::event::tcp_new_connect, [&](block::ModuleID target, block::ArgsPtr args) {
			connectorFD_ = args->pop_fd();

			if (!service_) {
				INFO_LOG("node connect root succ!");
				service_ = true;

				auto _args = block::ArgsPool::get_ref().get();
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
				eventRpc(block::event::coordinat_regist, getModuleID(), _args, [&](const block::ArgsPtr &args, int32_t progress, bool result) {
					if (result) {
						//dispatch(targetM_, block::event::node_create_succ, nullptr);
					}
				});
			}
		});

		listen(block::event::module_init_succ, [&](block::ModuleID target, block::ArgsPtr args) {
			/*
			auto _t = block::ArgsPool::get_ref().get();
			_t->push_block(args->pop_block(0, args->get_size()).c_str(), args->get_size());
			auto _module_id = _t->pop_moduleid();
			
			for (auto nod : eventMap_)
			{
				if (nod.second.connecotr_m_ == _module_id) {
					mailboxPtr_->dispatch(_module_id, eid::network::make_connector, block::makeArgs(getModuleID(), nod.second.ip_, nod.second.port_));
					mailboxPtr_->dispatch(nod.second.base_, eid::node::node_regist_succ, nullptr);
					break;
				}
			}
			*/
		});

	}
}

void block::modules::NodeModule::eRegistNode(block::ModuleID target, block::ArgsPtr args)
{
	auto _base = args->pop_i32();
	auto _event = args->pop_i32();
	auto _ip = args->pop_string();
	auto _port = args->pop_i32();

	registNode(_base, _event, _ip, _port);
}
