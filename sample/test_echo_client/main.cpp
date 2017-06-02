#include "cpp_test.h"
#include "lua_test.h"

void case_cpp_client(gsf::Application &app)
{
	for (int i = 0; i < 1; ++i)
	{
		app.regist_module(new Client);
	}
}

void case_lua_client(gsf::Application &app)
{
	app.regist_module(new gsf::modules::LuaProxyModule);
	app.regist_module(new PathModule);
	app.regist_module(new TestClientLuaModule);
}

int main()
{
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	gsf::Application app;
	gsf::AppConfig cfg;
	cfg.name = "test_echo_client";
	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule);
	app.regist_module(new gsf::network::ConnectorModule);

	//case_cpp_client(app);
	case_lua_client(app);

	app.run();

	return 0;
}
