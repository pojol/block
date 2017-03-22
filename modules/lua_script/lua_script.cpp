
#include "lua_script.h"


void gsf::modules::LuaScriptModule::init()
{
	listen(this, eid::lua_proxy::create
		, std::bind(&LuaScriptModule::create_impl, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, eid::lua_proxy::destroy
		, std::bind(&LuaScriptModule::destroy_impl, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, eid::lua_proxy::reload
		, std::bind(&LuaScriptModule::reload_impl, this
			, std::placeholders::_1
			, std::placeholders::_2));
}

void gsf::modules::LuaScriptModule::execute()
{
	//!
	for (auto itr : lua_map_)
	{
		lua_State *_L = itr.second;

		int _err = lua_pcall(_L, 0, 0, 0);
	}
}

void gsf::modules::LuaScriptModule::shut()
{

}

void gsf::modules::LuaScriptModule::create_impl(gsf::Args args, gsf::EventHandlerPtr callback)
{
	//ÏÈÑéÖ¤Âß¼­

	uint32_t _module_id = args.pop_uint32(0);
	std::string _path = args.pop_string(1);
	std::string _file = args.pop_string(2);

	lua_State *_L = luaL_newstate();
	luaL_openlibs(_L);

	int _err = luaL_dofile(_L, "F:\\github\\gsf\\sample\\test_script\\test_script.lua");
	lua_getglobal(_L, "execute");

	lua_map_.push_back(std::make_pair(_module_id, _L));
}

void gsf::modules::LuaScriptModule::destroy_impl(gsf::Args args, gsf::EventHandlerPtr callback)
{

}

void gsf::modules::LuaScriptModule::reload_impl(gsf::Args args, gsf::EventHandlerPtr callback)
{

}

