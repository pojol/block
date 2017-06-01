#include <core/application.h>
#include <core/event.h>

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

#include <lua_proxy/lua_proxy.h>
#include <log/log.h>
#include <timer/timer.h>

static char _path[512];
void get_bin_path()
{
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
}

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
		dispatch(eid::app_id, eid::get_module, gsf::Args("LuaProxyModule"), [&](const gsf::Args &args) {
			luaproxy_m_ = args.pop_int32(0);
		});
	}

	void init()
	{
		dispatch(luaproxy_m_, eid::lua_proxy::create, gsf::Args(get_module_id(), _path, "test_script.lua"));
	}

	void shut()
	{
		dispatch(luaproxy_m_, eid::lua_proxy::destroy, gsf::Args(get_module_id()));
	}

private :
	uint32_t luaproxy_m_ = 0;
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
	cfg.name = "test_script";
	app.init_cfg(cfg);

	get_bin_path();

	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new gsf::modules::TimerModule());

	app.regist_module(new gsf::modules::LuaProxyModule());
	app.regist_module(new TestLuaProxy());

	app.run();

	return 0;
}
