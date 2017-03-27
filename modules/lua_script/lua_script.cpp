
#include "lua_script.h"

#include <algorithm>

int gsf::modules::LuaScriptModule::pcall(lua_State *L, const char * func)
{
	int _ret = lua_getglobal(L, "module");
	if (lua_isnil(L, -1))
	{
		return 0;
	}

	_ret = lua_getfield(L, -1, func);
	if (lua_isnil(L, -1))
	{
		return 0;
	}

	if (lua_pcall(L, 0, 0, 0) != 0) {
		dispatch(log_module_, eid::log::error, gsf::Args(std::string(lua_tostring(L, -1))));
	}

	return 0;
}


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

	listen(this, eid::get_log_module, [&](gsf::Args args, gsf::EventHandlerPtr) {
		log_module_ = args.pop_uint32(0);
	});
}

void gsf::modules::LuaScriptModule::execute()
{
	//!
	for (auto itr : lua_map_)
	{
		lua_State *_L = std::get<1>(itr);

		pcall(_L, "execute");
		lua_settop(_L, 0);
	}
}

void gsf::modules::LuaScriptModule::shut()
{
	for (auto itr : lua_map_)
	{
		uint32_t _module_id = std::get<0>(itr);
		destroy(_module_id);
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

	if (_err != 0) {
		dispatch(log_module_, eid::log::error, gsf::Args(std::string(lua_tostring(_L, -1))));
	}

	pcall(_L, "init");
	lua_settop(_L, 0);

	lua_map_.push_back(std::make_tuple(module_id, _L, path));
}

void gsf::modules::LuaScriptModule::destroy_event(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _module_id = args.pop_uint32(0);
	destroy(_module_id);
}

int gsf::modules::LuaScriptModule::destroy(uint32_t module_id)
{
	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (std::get<0>(t) == module_id);
	});

	if (itr == lua_map_.end()) {
		return -1;
	}

	lua_State *_L = std::get<1>(*itr);

	pcall(_L, "shut");
	lua_settop(_L, 0);

	lua_close(_L);
	lua_map_.erase(itr);

	return 0;
}

void gsf::modules::LuaScriptModule::reload_event(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _module_id = args.pop_uint32(0);

	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (std::get<0>(t) == _module_id);
	});

	if (itr == lua_map_.end()) {
		return;
	}

	std::string _path = std::get<2>(*itr);

	if (destroy(_module_id) != 0) {
		return;
	}

	create(_module_id, _path);
}