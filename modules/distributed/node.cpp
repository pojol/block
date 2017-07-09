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
	});
}

void gsf::modules::NodeModule::init()
{
	using namespace std::placeholders;

	

	listen(this, eid::distributed::create_node, std::bind(&NodeModule::event_create_node, this, _1, _2));
}

void gsf::modules::NodeModule::execute()
{

}

void gsf::modules::NodeModule::shut()
{

}

void gsf::modules::NodeModule::event_create_node(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _isListen = args->pop_bool();
	accept_info_.name_ = args->pop_string();
	accept_info_.ip_ = args->pop_string();
	accept_info_.port_ = args->pop_i32();

	// 查看下是否已经开启了acceptor
	if (_isListen && acceptor_m_ == gsf::ModuleNil) {
		dispatch(eid::base::app_id, eid::base::new_dynamic_module, gsf::make_args("AcceptorModule"), [&](const gsf::ArgsPtr &args) {
			acceptor_m_ = args->pop_i32();
		});

		listen(this, eid::base::module_init_succ, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			auto _m = args->pop_i32();
			if (_m == acceptor_m_) {
				dispatch(acceptor_m_, eid::network::make_acceptor, gsf::make_args(get_module_id(), accept_info_.ip_, accept_info_.port_));
			}
		});

		listen(this, eid::network::new_connect, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
		});

		listen(this, eid::network::dis_connect, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
		});
	}

	auto _num = args->pop_i32();
	for (int i = 0; i < _num; ++i)
	{
		auto _connect_name = args->pop_string();
		auto _connect_ip = args->pop_string();
		auto _connect_port = args->pop_i32();

		//检查下是否有同名的module
		auto _fitr = std::find_if(conn_vec_.begin(), conn_vec_.end(), [&](std::vector<ConnInfo>::value_type it) {
			return (it.name_ == _connect_name);
		});
		if (_fitr != conn_vec_.end()) {
			dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogErr, "NodeModule", "create node connect name repeat!"));
			return;
		}

		dispatch(eid::base::app_id, eid::base::new_dynamic_module, gsf::make_args("ConnectorModule"), [&](const gsf::ArgsPtr &args) {
			ConnInfo _info;
			_info.m_ = args->pop_i32();
			_info.name_ = _connect_name;
			conn_vec_.push_back(_info);
		});
	}

}

