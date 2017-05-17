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

#include <module/application.h>
#include <event/event.h>

#include <network/acceptor.h>
#include <network/connector.h>

#include <log/log.h>

#include <iostream>

#include <random>

#include "../../common/single.h"

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
		dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](gsf::Args args) {
			log_ = args.pop_uint32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args("Client2LoginServer"), [&](gsf::Args args) {
			client2login_ = args.pop_uint32(0);
		});
	}

	void init()
	{
		char _path[512];
#ifdef WIN32
		GetModuleFileName(NULL, _path, 512);
		//取出文件路径
		for (int i = strlen(_path); i >= 0; i--)
		{
			if (_path[i] == '\\')
			{
				_path[i] = '\0';
				break;
			}
		}
#else
		int cnt = readlink("/proc/self/exe", _path, 512);
		if (cnt < 0 || cnt >= 512) {
			std::cout << "read path err" << std::endl;
			return;
		}
		for (int i = cnt; i >= 0; --i)
		{
			if (_path[i] == '/') {
				_path[i + 1] = '\0';
				break;
			}
		}
#endif // WIN32

		dispatch(log_, eid::log::init, gsf::Args(std::string(_path) + "/log", "echo_server"));

		//test
		listen(this, eid::network::new_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			dispatch(log_, eid::log::info, gsf::Args("new connect fd : ", args.pop_uint32(0)));
		});

		listen(this, eid::network::dis_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			dispatch(log_, eid::log::info, gsf::Args("dis connect fd : ", args.pop_uint32(0)));
		});

		dispatch(client2login_, eid::network::send_remote_callback, gsf::Args(), [&](gsf::Args args) {
			send_ = args.pop_remote_callback(0);
		});

		dispatch(client2login_, eid::network::make_acceptor, gsf::Args(get_module_id(), "127.0.0.1", uint32_t(8001)));
	
		auto arr = {
			std::make_pair(uint32_t(1001), std::bind(&EntityMgr::test_remote, this
				, std::placeholders::_1)),
		};

		for (auto nod : arr)
		{
			//! 向协议绑定器申请，module 和 协议的绑定.
			dispatch(client2login_, eid::network::recv_remote_callback
				, gsf::Args(get_module_id(), uint32_t(1001)), std::bind(&EntityMgr::test_remote, this, std::placeholders::_1));
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

	void test_remote(gsf::Args args)
	{
		auto fd = args.pop_uint32(0);

		//dispatch(log_, eid::log::info, gsf::Args(str));
		second_pack_num_++;
		//_info.set_name("world");
		//Face.send_msg<Client2LoginServer>(this, fd, 1002, _info);
		send_(fd, 1002, "gsf");
	}

private :
	uint32_t tick_len_;
	int32_t last_tick_;

	uint32_t second_pack_num_;

	ModuleID log_ = ModuleNil;
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

	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new Client2LoginServer());
	app.regist_module(new EntityMgr());

	app.run();

	return 0;
}
