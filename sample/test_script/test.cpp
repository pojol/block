#include <module/application.h>
#include <event/event.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif // WIN32

#include <lua_script/lua_script.h>
#include <log/log.h>

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
		dispatch2<gsf::modules::LogModule>(eid::log::init, gsf::Args(std::string(_path)));
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
	~TestLuaProxy() {}

	void init()
	{
		dispatch(Face.get_module_id<gsf::modules::LuaScriptModule>()
			, eid::get_log_module
			, gsf::Args(Face.get_module_id<gsf::modules::LogModule>()));

		//test
		dispatch(Face.get_module_id<gsf::modules::LuaScriptModule>(), eid::lua_proxy::create
			, gsf::Args(get_module_id()
			, std::string("E:/github/gsf/sample/test_script/test_script.lua")));
	}

	void shut()
	{
		dispatch(Face.get_module_id<gsf::modules::LuaScriptModule>(), eid::lua_proxy::destroy
			, gsf::Args(get_module_id()));
	}
};

int main()
{
	auto appptr = new gsf::Application;
	new gsf::EventModule;
	new AppFace;

	appptr->regist_module(gsf::EventModule::get_ptr());
	appptr->regist_module(new gsf::modules::LogModule);
	appptr->regist_module(new gsf::modules::LuaScriptModule);
	appptr->regist_module(new TestLuaProxy);

	Face.init(appptr);
	appptr->run();

	return 0;
}
