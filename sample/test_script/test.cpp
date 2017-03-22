#include <module/application.h>
#include <event/event.h>

#include <lua_script/lua_script.h>

class AppFace
	: public gsf::utils::Singleton<AppFace>
	, public gsf::IEvent
{
public:

	void init(gsf::Application *app)
	{
		app_ = app;
	}

	template <typename M>
	uint32_t get_module_id();

private:
	gsf::Application *app_;
};

template <typename M>
uint32_t AppFace::get_module_id()
{
	return app_->find_module_id<M>();
}

#define Face AppFace::get_ref()


class TestLuaModule
	: public gsf::Module
	, public gsf::IEvent
{
public:

	void init()
	{
		//test
		dispatch(Face.get_module_id<gsf::modules::LuaScriptModule>(), eid::lua_proxy::create
			, gsf::Args(get_module_id()
			, std::string("E:/github/gsf/sample/test_script/test_script.lua")));
	}

	
};

int main()
{
	auto appptr = new gsf::Application;
	new gsf::EventModule;
	new AppFace;

	appptr->regist_module(gsf::EventModule::get_ptr());
	appptr->regist_module(new gsf::modules::LuaScriptModule);
	appptr->regist_module(new TestLuaModule);

	Face.init(appptr);
	appptr->run();

	return 0;
}
