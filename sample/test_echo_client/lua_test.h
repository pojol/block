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
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#include <lua_proxy/lua_proxy.h>
#include <log/log.h>
#include <timer/timer.h>

class PathModule
	: public gsf::Module
	, public gsf::IEvent
{
public:

	PathModule()
		: Module("PathModule")
	{}

	virtual ~PathModule() {}

	void before_init()
	{
		char _path[512];

#ifdef WIN32
		GetModuleFileName(NULL, _path, 512);
		//ȡ���ļ�·��
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

		path_ = _path;

		listen(this, eid::sample::get_proc_path, [=](const gsf::Args &args, gsf::CallbackFunc callback) {
			callback(gsf::Args(path_));
		});
	}

private:
	std::string path_ = "";
};

class TestClientLuaModule
	: public gsf::Module
	, public gsf::IEvent
{
public:
	TestClientLuaModule()
		: Module("TestClientLuaModule")
	{}

	virtual ~TestClientLuaModule() {}

	void before_init()
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args("LuaProxyModule"), [&](const gsf::Args &args) {
			luaproxy_m_ = args.pop_int32(0);
		});

		gsf::ModuleID _path_m = 0;
		dispatch(eid::app_id, eid::get_module, gsf::Args("PathModule"), [&](const gsf::Args &args) {
			_path_m = args.pop_int32(0);
		});

		dispatch(_path_m, eid::sample::get_proc_path, gsf::Args(), [&](const gsf::Args &_args) {
			path_ = _args.pop_string(0);
		});
	}

	void init()
	{
		dispatch(luaproxy_m_, eid::lua_proxy::create, gsf::Args(get_module_id(), path_, "client.lua"));
	}

	void shut()
	{
		dispatch(luaproxy_m_, eid::lua_proxy::destroy, gsf::Args(get_module_id()));
	}

private:
	uint32_t luaproxy_m_ = 0;
	std::string path_ = "";
};


namespace gsf
{
	namespace network
	{
		REGISTER_CLASS(ConnectorModule)
	}
}