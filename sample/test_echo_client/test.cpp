#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sstream>


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

#include <log/log.h>

#include <random>
#include <lua_proxy/lua_proxy.h>


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

using namespace gsf;

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
		dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](const gsf::Args &args) {
			log_ = args.pop_int32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args("LuaProxyModule"), [&](const gsf::Args &args) {
			lua_ = args.pop_int32(0);
		});
	}

	void init()
	{

		//test
		dispatch(lua_, eid::lua_proxy::create
			, gsf::Args(get_module_id(), _path, "client.lua"));
	}

	void shut()
	{
		dispatch(lua_, eid::lua_proxy::destroy, gsf::Args(get_module_id()));
	}

private:
	ModuleID log_ = ModuleNil;
	ModuleID lua_ = ModuleNil;
};

namespace gsf
{
	namespace network
	{
		REGISTER_CLASS(ConnectorModule)
	}
}


class Client
	: public gsf::Module
	, public gsf::IEvent
{
public:
	Client()
		: Module("Client")
	{}

	void before_init() override
	{
		dispatch(eid::app_id, eid::new_dynamic_module, gsf::Args("ConnectorModule"), [&](const gsf::Args &args){
			connector_id_ = args.pop_int32(0);

			listen(connector_id_, eid::network::connector_init, std::bind(&Client::create_connector_succ, this, std::placeholders::_1));
		});

	}

	void create_connector_succ(const gsf::Args &args)
	{
		dispatch(connector_id_, eid::network::send_remote_callback, gsf::Args(), [&](const gsf::Args &args) {
			send_ = args.pop_remote_callback(0);
		});

		listen(this, eid::network::new_connect, [&](const gsf::Args &args, gsf::CallbackFunc callback) {
			fd_ = args.pop_int32(0);

			send_(fd_, 1001, "hello");
		});

		dispatch(connector_id_, eid::network::recv_remote_callback, gsf::Args(
			  get_module_id()
			, 1002), std::bind(&Client::msg_handler, this, std::placeholders::_1));

		dispatch(connector_id_, eid::network::make_connector
			, gsf::Args(get_module_id(), "127.0.0.1", 8001));
	}

	void init() override 
	{
		
	}

	void msg_handler(const gsf::Args &args)
	{
		send_(fd_, 1001, "hello");
	}

private:
	ModuleID connector_id_ = ModuleNil;
	SessionID fd_ = SessionNil;

	gsf::RemoteFunc send_;
};


int main()
{
#ifdef WIN32
    WSADATA wsa_data;
    WSAStartup(0x0201, &wsa_data);
#endif

	get_bin_path();

	gsf::Application app;
	gsf::AppConfig cfg;
	cfg.name = "test_echo_client";
	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new gsf::network::ConnectorModule());
	//app.regist_module(new gsf::modules::LuaProxyModule());
	//app.regist_module(new Login_LuaProxy());

	for (int i = 0; i < 1; ++i)
	{
		app.regist_module(new Client);
	}
	
	app.run();

	return 0;
}
