#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>


#ifdef WIN32
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <unistd.h>
#endif // WIN32

#include <core/application.h>
#include <core/event.h>

#include <network/acceptor.h>
#include <network/connector.h>

#include <log/log.h>

#include <iostream>

#include <random>

#include "addressbook.pb.h"

using namespace gsf;

class Client2LoginServer
	: public gsf::network::AcceptorModule
{
public:
	Client2LoginServer()
		: AcceptorModule("Client2LoginServer")
	{}

private:

};

class EntityMgr
	: public gsf::Module
	, public gsf::IEvent
{
public:

	EntityMgr()
		: Module("EntityMgr")
	{
		tick_len_ = 20;	// one second
		last_tick_ = -1;
		second_pack_num_ = 0;
	}

	void before_init() override
	{
		dispatch(eid::app_id, eid::get_module, gsf::make_args("LogModule"), [&](const gsf::ArgsPtr &args) {
			log_m_ = args->pop_i32();
		});

		dispatch(eid::app_id, eid::get_module, gsf::make_args("Client2LoginServer"), [&](const gsf::ArgsPtr &args) {
			client2login_ = args->pop_i32();
		});
	}

	void init() override
	{
		//test
		listen(this, eid::network::new_connect
			, [=](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogInfo, "test", "new connect fd = ", args->pop_fd()));
		});

		listen(this, eid::network::dis_connect
			, [=](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			dispatch(log_m_, eid::log::print, gsf::make_log(gsf::LogInfo, "test", "dis connect fd = ", args->pop_fd()));
		});

		dispatch(client2login_, eid::network::make_acceptor, gsf::make_args(get_module_id(), "127.0.0.1", 8001));

		listen(this, eid::network::recv, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			auto _fd = args->pop_fd();
			auto _msgid = args->pop_msgid();

			if (1001 == _msgid) {
				auto block = args->pop_string();

				tutorial::Person _p;
				_p.ParsePartialFromString(block);

				std::cout << _p.name() << " " << _p.id() << " " << _p.email() << std::endl;
			}
		});
	}

	void execute() override
	{
		int _t = (last_tick_ + 1) % tick_len_;
		if (_t == 0) {
			std::cout << "package num : " << second_pack_num_ << std::endl;
			second_pack_num_ = 0;
		}
		last_tick_ = _t;
	}

private :
	uint32_t tick_len_;
	int32_t last_tick_;

	uint32_t second_pack_num_;

	ModuleID log_m_ = ModuleNil;

	ModuleID client2login_ = ModuleNil;

};

int main()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0){
		exit(1);
	}
#endif // WIN32

	gsf::Application app;
	gsf::AppConfig cfg;
	cfg.is_watch_pref = true;
	cfg.tick_count = 50;
	cfg.name = "test_echo_server";

	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new Client2LoginServer());
	app.regist_module(new EntityMgr());

	app.run();

	return 0;
}
