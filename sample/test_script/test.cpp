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

class AppFace
	: public gsf::utils::Singleton<AppFace>
	, public gsf::IEvent
{
public:

	~AppFace(){}

	void init(gsf::Application *app)
	{
		app_ = app;
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

		//test
#else
		int cnt = readlink("/proc/self/exe", _path, 512);
		if (cnt < 0 || cnt >= 512){
			std::cout << "read path err" << std::endl;
			return;
		}
		for (int i = cnt; i >=0; --i)
		{
			if (_path[i] == '/'){
				_path[i+1] = '\0';
				break;
			}
		}
#endif

		dispatch2<gsf::modules::LogModule>(eid::log::init, gsf::Args(std::string(_path)
			, std::string("test_script")));
	}

	template <typename M>
	uint32_t get_module_id();

	template <typename M>
	void dispatch2(uint32_t eid, gsf::Args args);

private:
	gsf::Application *app_;
};

template <typename M>
void AppFace::dispatch2(uint32_t eid, gsf::Args args)
{
	dispatch(app_->find_module_id<M>(), eid, args);
}

template <typename M>
uint32_t AppFace::get_module_id()
{
	return app_->find_module_id<M>();
}

#define Face AppFace::get_ref()


class TestLuaProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	TestLuaProxy()
		: Module("TestLuaProxy")
	{}

	~TestLuaProxy() {}

	void init()
	{
		//test
		dispatch(Face.get_module_id<gsf::modules::LuaScriptModule>(), eid::lua_proxy::create
			, gsf::Args(get_module_id()
			, std::string("F:/github/gsf/sample/test_script/test_script.lua")));

		dispatch(get_module_id(), 10001, gsf::Args(uint32_t(10)));
	}

	void shut()
	{
		dispatch(Face.get_module_id<gsf::modules::LuaScriptModule>(), eid::lua_proxy::destroy
			, gsf::Args(get_module_id()));
	}
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

	void init()
	{
		uint32_t _c2l_id = Face.get_module_id<Client2LoginServer>();

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
		Face.dispatch2<Client2LoginServer>(eid::network::make_acceptor, args);
	}
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

	auto appptr = new gsf::Application();
	new AppFace;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	appptr->regist_module(new gsf::modules::LogModule());
	appptr->regist_module(new gsf::modules::TimerModule());

	appptr->regist_module(new Client2LoginServer());
	appptr->regist_module(new Client2LoginProxy());

	appptr->regist_module(new gsf::modules::LuaScriptModule());
	appptr->regist_module(new TestLuaProxy());

	Face.init(appptr);
	appptr->init();
	appptr->run();

	return 0;
}
