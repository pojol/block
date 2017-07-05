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
		dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](const gsf::ArgsPtr &args) {
			log_m_ = args.pop_int32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args("Client2LoginServer"), [&](const gsf::ArgsPtr &args) {
			client2login_ = args.pop_int32(0);
		});
	}

	void init() override
	{
		dispatch(log_m_, eid::log::log_callback, gsf::Args(), [&](const gsf::ArgsPtr &args) {
			log_f_ = args.pop_log_callback(0);
		});


		//test
		listen(this, eid::network::new_connect
			, [=](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			log_f_(eid::log::info, "EntityMgr", gsf::Args("new connect fd : ", args.pop_int32(0)));
		});

		listen(this, eid::network::dis_connect
			, [=](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {
			log_f_(eid::log::info, "EntityMgr", gsf::Args("dis connect fd : ", args.pop_int32(0)));
		});

		dispatch(client2login_, eid::network::send_remote_callback, gsf::Args(), [&](const gsf::ArgsPtr &args) {
			send_ = args.pop_remote_callback(0);
		});

		dispatch(client2login_, eid::network::make_acceptor, gsf::Args(get_module_id(), "127.0.0.1", 8001));
	

		// bind msg
		auto arr = {
			std::make_pair(1001, std::bind(&EntityMgr::test_remote, this
				, std::placeholders::_1)),
		};

		for (auto nod : arr)
		{
			//! 向协议绑定器申请，module 和 协议的绑定.
			dispatch(client2login_, eid::network::recv_remote_callback
				, gsf::Args(get_module_id(), 1001)
				, std::bind(&EntityMgr::test_remote, this, std::placeholders::_1));
		}
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

	void test_remote(const gsf::ArgsPtr &args)
	{
		auto fd = args.pop_int32(0);
		auto block = args.pop_string(2);

		tutorial::Person _p;
		_p.ParsePartialFromString(block);

		std::cout << _p.name() << " " << _p.id() << " " << _p.email() << std::endl;

		//dispatch(log_, eid::log::info, gsf::Args(str));
		//second_pack_num_++;
		//_info.set_name("world");
		//Face.send_msg<Client2LoginServer>(this, fd, 1002, _info);
		//send_(fd, 1002, "gsf");
	}

private :
	uint32_t tick_len_;
	int32_t last_tick_;

	uint32_t second_pack_num_;

	ModuleID log_m_ = ModuleNil;
	gsf::LogFunc log_f_ = nullptr;

	ModuleID client2login_ = ModuleNil;

	gsf::RemoteFunc send_;
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
