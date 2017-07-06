#include "cpp_test.h"
#include "lua_test.h"


void case_cpp_timer(gsf::Application &app)
{
	app.regist_module(new TestCaseModule);
}


void case_lua_timer(gsf::Application &app)
{
	app.regist_module(new PathModule);
	app.regist_module(new gsf::modules::LuaProxyModule);
	app.regist_module(new TestCaseLuaModule);
}


int main()
{
	gsf::Application app;
	gsf::AppConfig cfg;
	//cfg.is_watch_pref = true;
	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule);
	app.regist_module(new gsf::modules::TimerModule);

	case_cpp_timer(app);
	//case_lua_timer(app);

	app.run();

	return 0;
}
