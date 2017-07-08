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

#include <distributed\node.h>

#include <log/log.h>

#include <random>

class LoginModule
	: public gsf::Module
	, public gsf::IEvent
{
public:
	LoginModule()
		: Module("LoginModule")
	{}

	void before_init() override
	{
		dispatch(eid::app_id, eid::get_module, gsf::make_args("NodeModule"), [&](const gsf::ArgsPtr &args) {
			node_m_ = args->pop_i32();
		});
	}

	void init() override
	{
		// 暂时用动态构建，后面复杂的结构要支持通过配置构建
		// 1. 是否开启listen 2. ip 3 prot 4 connect num 5 [name ip port] 
		dispatch(node_m_, eid::distributed::create_node, gsf::make_args(true, "Login", "127.0.0.1", 8001, 1, "Root", "127.0.0.1", 10000));
	}

private:
	gsf::ModuleID node_m_ = gsf::ModuleNil;
};


int main()
{
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	gsf::Application app;
	gsf::AppConfig cfg;
	cfg.name = "test_echo_client";
	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule);
	app.regist_module(new gsf::modules::NodeModule);
	app.regist_module(new LoginModule);

	app.run();

	return 0;
}