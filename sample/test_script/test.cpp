#include <module/application.h>
#include <event/event.h>

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

#include <lua_script/lua_script.h>
#include <log/log.h>
#include <timer/timer.h>

#include <network/acceptor.h>
#include <network/connector.h>

#include "test.pb.h"

class TestLuaProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	TestLuaProxy()
		: Module("TestLuaProxy")
	{}

	~TestLuaProxy() {}

	void before_init()
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args("LuaScriptModule"), [&](gsf::Args args) {
			lua_binder_ = args.pop_uint32(0);
		});
	}

	void init()
	{
		//test
		dispatch(lua_binder_, eid::lua_proxy::create
			, gsf::Args(get_module_id()
			, std::string("F:/github/gsf/sample/test_script/test_script.lua")));

		dispatch(get_module_id(), 10001, gsf::Args(uint32_t(10)));
	}

	void shut()
	{
		dispatch(lua_binder_, eid::lua_proxy::destroy, gsf::Args(get_module_id()));
	}

private :
	uint32_t lua_binder_;
};

class Client2LoginServer
	: public gsf::network::AcceptorModule
{
public:
	Client2LoginServer()
		: AcceptorModule("Client2LoginServer")
	{}

private:

};

class Client2LoginProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	Client2LoginProxy()
		: Module("Client2LoginProxy")
	{}

	void before_init()
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args("Client2LoginServer"), [&](gsf::Args args) {
			client2login_ = args.pop_uint32(0);
		});
	}

	void init()
	{
		listen(this, eid::network::new_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
		});

		listen(this, eid::network::dis_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_module_id() << std::string("127.0.0.1") << uint32_t(8001);
		dispatch(client2login_, eid::network::make_acceptor, args);
	}

private :
	uint32_t client2login_;
};

int main()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0) {
		exit(1);
	}
#endif // WIN32

	gsf::Application app;
	gsf::AppConfig cfg;
	app.init_cfg(cfg);

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new gsf::modules::TimerModule());

	app.regist_module(new Client2LoginServer());
	app.regist_module(new Client2LoginProxy());

	app.regist_module(new gsf::modules::LuaScriptModule());
	app.regist_module(new TestLuaProxy());

	app.run();

	return 0;
}
