#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sstream>
#include <iostream>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#include <core/application.h>
#include <core/event.h>
#include <core/dynamic_module_factory.h>

#include <network/acceptor.h>
#include <network/connector.h>

#include <log/log.h>

#include <random>
#include <lua_proxy/lua_proxy.h>

#include "addressbook.pb.h"

class Client
	: public gsf::Module
	, public gsf::IEvent
{
public:
	Client()
		: Module("Client")
	{}

	void before_init() override
	{
		dispatch(eid::app_id, eid::new_dynamic_module, gsf::make_args("ConnectorModule"), [&](const gsf::ArgsPtr &args) {
			connector_id_ = args->pop_i32();

			listen(connector_id_, eid::base::module_init_succ, std::bind(&Client::create_connector_succ, this, std::placeholders::_1));
		});

	}

	void create_connector_succ(const gsf::ArgsPtr &args)
	{
		auto _module_id = args->pop_moduleid();
		if (_module_id != connector_id_) { return; }

		listen(this, eid::network::new_connect, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			fd_ = args->pop_fd();

			tutorial::Person _p;
			_p.set_name("jack");
			_p.set_id(1000);
			_p.set_email("127.0.0.1");
			std::string _msg = "";

			if (_p.SerializeToString(&_msg)) {
				dispatch(connector_id_, eid::network::send, gsf::make_args(1001, _msg));
			}

			// 分布式rpc调用接口预定义
			//dispatch(node_m_, eid::login::auth, gsf::make_args("account", "password", "verify_key"));

		});
		
		listen(this, eid::network::recv, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			
			auto _fd = args->pop_fd();
			auto _msgid = args->pop_msgid();
			if (_msgid == 1002) {
					
				std::cout << "recv msg = 1002" << std::endl;

			}
		});

		dispatch(connector_id_, eid::network::make_connector
			, gsf::make_args(get_module_id(), "127.0.0.1", 8001));
	}

	void init() override
	{

	}

private:
	gsf::ModuleID connector_id_ = gsf::ModuleNil;
	gsf::SessionID fd_ = gsf::SessionNil;
};