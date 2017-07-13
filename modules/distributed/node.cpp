#include "node.h"

#include <modules\network\acceptor.h>
#include <modules\network\connector.h>

#include <core/dynamic_module_factory.h>

#include <algorithm>


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
	dispatch(eid::base::app_id, eid::get_module, gsf::make_args("LogModule"), [&](const gsf::ArgsPtr &args) {
		log_m_ = args->pop_i32();
		if (log_m_ == gsf::ModuleNil) {
			printf("lack of dependency on LogModule\n");
		}
	});

	dispatch(eid::base::app_id, eid::get_module, gsf::make_args("TimerModule"), [&](const gsf::ArgsPtr &args) {
		timer_m_ = args->pop_i32();
		if (timer_m_ == gsf::ModuleNil) {
			dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogErr, "lack of dependency on TimerModule!"));
		}
	});

	using namespace std::placeholders;
	listen(this, eid::distributed::create_node, std::bind(&NodeModule::event_create_node, this, _1, _2));

	rpc_listen(std::bind(&NodeModule::event_rpc, this, _1, _2, _3, _4));
}

void gsf::modules::NodeModule::init()
{
	listen(this, eid::network::recv, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
	
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
	});

	listen(this, eid::timer::timer_arrive, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
		auto _tid = args->pop_ui64();

		auto _itr = timer_set_.find(_tid);
		if (_itr != timer_set_.end()) { // timeout

			auto _info = _itr->second;

			timer_set_.erase(_itr);

			auto _citr = callback_map_.find(_info->event_);
			_info->callback(nullptr, false);
			callback_map_.erase(_citr);
		}
	});
}

void gsf::modules::NodeModule::execute()
{

}

void gsf::modules::NodeModule::shut()
{

}

void gsf::modules::NodeModule::event_rpc(const std::string &module, uint32_t event, const gsf::ArgsPtr &args, gsf::RpcCallback callback)
{
	auto _itr = callback_map_.find(event);
	if (_itr != callback_map_.end()) {
		dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogWarning, "repeat rpc event!"));
		return;
	}

	if (callback) {
		uint64_t _tid = 0;
		dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(rpc_delay_), [&](const gsf::ArgsPtr &args) {
			_tid = args->pop_ui64();
		});

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
		argsPtr->push(module);
		argsPtr->push_block(args->pop_block(0, args->get_size()).c_str(), args->get_size());
		dispatch(connector_m_, eid::network::send, std::move(argsPtr));
	}
	else {
		dispatch(connector_m_, eid::network::send, gsf::make_args(event));
	}
}

void gsf::modules::NodeModule::event_create_node(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
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
			auto _module_name = args->pop_string();
			auto _module_id = args->pop_i32();

			// £¡¼ì²é
			// todo 
			modules_.push_back(std::make_pair(_module_name, _module_id));
		}

		dispatch(eid::base::app_id, eid::base::new_dynamic_module, gsf::make_args("ConnectorModule"), [&](const gsf::ArgsPtr &args) {
			connector_m_ = args->pop_moduleid();
		});


		listen(this, eid::network::new_connect, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			connector_fd_ = args->pop_fd();

			dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogInfo, "distributed nod connect 2 root!"));
			service_ = true;

			auto _args = gsf::ArgsPool::get_ref().get();
			_args->push(type_);
			_args->push(acceptor_ip_);
			_args->push(acceptor_port_);
			_args->push(int32_t(modules_.size()));
			for (auto &it : modules_)
			{
				_args->push(it.first);
				_args->push(it.second);
			}
			event_rpc("CoodinatorModule", eid::distributed::coordinat_regist, _args, [&](const gsf::ArgsPtr &args, bool result) {
				if (result) {
					dispatch(target_m_, eid::distributed::create_node_succ, nullptr);
				}
			});
		});


		listen(this, eid::base::module_init_succ, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			auto _module_id = args->pop_moduleid();

			if (_module_id == connector_m_) {

				dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogInfo, "distributed nod connector init success!"));

				dispatch(connector_m_, eid::network::make_connector, gsf::make_args(get_module_id(), root_ip_, root_port_));
			}
		});
	}

}

