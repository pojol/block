
#include "lua_script.h"

#include <algorithm>

void gsf::modules::LuaScriptModule::init()
{
	listen(this, eid::lua_proxy::create
		, std::bind(&LuaScriptModule::create_event, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, eid::lua_proxy::destroy
		, std::bind(&LuaScriptModule::destroy_event, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, eid::lua_proxy::reload
		, std::bind(&LuaScriptModule::reload_event, this
			, std::placeholders::_1
			, std::placeholders::_2));
}

void gsf::modules::LuaScriptModule::execute()
{
	//!
	for (auto itr : lua_map_)
	{
		lua_State *_L = itr.second;

		lua_getfield(_L, -1, "execute");
		if (lua_isnil(_L, -1)) {
			return;
		}

		int _ret = lua_pcall(_L, 0, 0, 0);
		if (_ret != 0) {
			//
		}
	}
}

void gsf::modules::LuaScriptModule::shut()
{
	for (auto itr : lua_map_)
	{
		destroy(itr.first);
	}

	lua_map_.clear();
}

void gsf::modules::LuaScriptModule::create_event(gsf::Args args, gsf::EventHandlerPtr callback)
{
	//ÏÈÑéÖ¤Âß¼­

	uint32_t _module_id = args.pop_uint32(0);
	std::string _path = args.pop_string(1);

	create(_module_id, _path);
}

void gsf::modules::LuaScriptModule::create(uint32_t module_id, std::string path)
{
	lua_State *_L = luaL_newstate();
	luaL_openlibs(_L);

	int _err = luaL_dofile(_L, path.c_str());

	_err = lua_getglobal(_L, "module");
	if (lua_isnil(_L, -1)) {
		return;
	}

	lua_getfield(_L, -1, "init");
	if (lua_isnil(_L, -1)) {
		return;
	}

	int _ret = lua_pcall(_L, 0, 0, 0);
	if (_ret != 0) {
		//
	}

	lua_map_.push_back(std::make_pair(module_id, _L));
}

void gsf::modules::LuaScriptModule::destroy_event(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _module_id = args.pop_uint32(0);
	destroy(_module_id);
}

void gsf::modules::LuaScriptModule::destroy(uint32_t module_id)
{
	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (t.first == module_id);
	});

	if (itr == lua_map_.end()) {
		return;
	}

	lua_State *_L = itr->second;

	lua_getfield(_L, -1, "shut");
	if (lua_isnil(_L, -1)) {
		return;
	}

	int _ret = lua_pcall(_L, 0, 0, 0);
	if (_ret != 0) {
		//
	}

	lua_close(_L);
	lua_map_.erase(itr);
}

void gsf::modules::LuaScriptModule::reload_event(gsf::Args args, gsf::EventHandlerPtr callback)
{

}

