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

#include <module/dynamic_module_factory.h>

#include <log/log.h>

#include <iostream>

#include <random>
#include <lua_script/lua_script.h>

#include "../../common/single.h"



namespace gsf
{
	namespace network
	{
		REGISTER_CLASS(ConnectorModule)
	}
}

class Login_LuaProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	Login_LuaProxy()
		: Module("Login_LuaProxy")
	{}

	~Login_LuaProxy() {}

	void before_init() override
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("LogModule")), [&](gsf::Args args) {
			log_ = args.pop_uint32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("LuaScriptModule")), [&](gsf::Args args) {
			lua_ = args.pop_uint32(0);
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
			, std::string("echo_client")));

		//test
		dispatch(lua_, eid::lua_proxy::create
			, gsf::Args(get_module_id(), std::string("F:/github/gsf/sample/test_echo_client/client.lua")));
	}

	void shut()
	{
		dispatch(lua_, eid::lua_proxy::destroy
			, gsf::Args(get_module_id()));
	}

private:
	uint32_t log_;
	uint32_t lua_;
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
	app.regist_module(new gsf::modules::LuaScriptModule());
	app.regist_module(new Login_LuaProxy());

	app.init();
	app.run();

	return 0;
}
