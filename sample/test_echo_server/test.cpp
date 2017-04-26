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
	{}

	void before_init() override
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("LogModule")), [&](gsf::Args args) {
			log_ = args.pop_uint32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("Client2LoginServer")), [&](gsf::Args args) {
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

		//test
		
		dispatch(log_, eid::log::init, gsf::Args(std::string(_path)
			, std::string("echo_server")));

		auto arr = {
			std::make_pair(uint32_t(1001), std::bind(&EntityMgr::test_remote, this
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3)),
		};

		for (auto nod : arr)
		{
			//! 向协议绑定器申请，module 和 协议的绑定.
			dispatch(eid::app_id, eid::network::bind_remote_callback
				, gsf::Args(get_module_id(), nod.first, nod.second));
		}
	}

	void test_remote(uint32_t fd, uint32_t msg_id, std::string str)
	{
		dispatch(log_, eid::log::info, gsf::Args(str));

		//_info.set_name("world");
		//Face.send_msg<Client2LoginServer>(this, fd, 1002, _info);
		dispatch_remote(client2login_, fd, 1002, "gsf");
	}

private :
	uint32_t log_;
	uint32_t client2login_;
};

class Client2LoginProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	Client2LoginProxy()
		: Module("Client2LoginProxy")
	{}

	void before_init() override
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("Client2LoginServer")), [&](gsf::Args args) {
			client2login_ = args.pop_uint32(0);
		});
	}

	void init() override
	{
		listen(this, eid::network::new_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
		});

		listen(this, eid::network::dis_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback){
			std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_module_id() << std::string("127.0.0.1") << uint32_t(8001);
		dispatch(client2login_, eid::network::make_acceptor, args);
	}

private:
	uint32_t client2login_;
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

	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new Client2LoginServer());
	app.regist_module(new Client2LoginProxy());
	app.regist_module(new EntityMgr());

	app.init();
	app.run();

	return 0;
}
