
#include "lua_script.h"

#include <algorithm>

std::string Traceback(lua_State * _state)
{
	lua_Debug info;
	int level = 0;
	std::string outputTraceback;
	char buffer[4096];

	while (lua_getstack(_state, level, &info)) 
	{
		lua_getinfo(_state, "nSl", &info);

#if defined(_WIN32) && defined(_MSC_VER)
		sprintf_s(buffer, "  [%d] %s:%d -- %s [%s]\n",
			level, info.short_src, info.currentline,
			(info.name ? info.name : "<unknown>"), info.what);
#else
		sprintf(buffer, "  [%d] %s:%d -- %s [%s]\n",
			level, info.short_src, info.currentline,
			(info.name ? info.name : "<unknown>"), info.what);
#endif
		outputTraceback.append(buffer);
		++level;
	}
	return outputTraceback;
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

	dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("LogModule")), [=](gsf::Args args)
	{
		log_module_ = args.pop_uint32(0);
	});
}

void gsf::modules::LuaScriptModule::execute()
{
	//!
	for (auto itr = lua_map_.begin(); itr != lua_map_.end();)
	{
		LuaProxy *_lua = itr->second;

		try {
			sol::table _module = _lua->state_.get<sol::table>("module");

			if (_lua->app_state_ == LuaAppState::INIT) {
				_lua->call_list_[LuaAppState::INIT](_module);
				_lua->app_state_ = LuaAppState::EXECUTE;
			}
			else if (_lua->app_state_ == LuaAppState::EXECUTE) {
				_lua->call_list_[LuaAppState::EXECUTE](_module);
			}
			else if (_lua->app_state_ == LuaAppState::SHUT) {
				_lua->call_list_[LuaAppState::SHUT](_module);
				_lua->app_state_ = LuaAppState::AFTER_SHUT;
			}
			else if (_lua->app_state_ = LuaAppState::AFTER_SHUT) {
				_lua->call_list_[LuaAppState::AFTER_SHUT](_module);
			}
		}
		catch (sol::error e) {
			dispatch(log_module_, eid::log::error, gsf::Args(std::string(e.what())));
		}

		if (_lua->app_state_ == LuaAppState::AFTER_SHUT) {

			//lua_close(_lua->state_.lua_state());  sol 会在析构的时候释放 lua_state

			//delete _lua;		//bug
			//_lua = nullptr;

			itr = lua_map_.erase(itr);
		}
		else {
			++itr;
		}
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

void gsf::modules::LuaScriptModule::create_event(gsf::Args args, gsf::CallbackFunc callback)
{
	//ÏÈÑéÖ¤Âß¼­
	uint32_t _module_id = args.pop_uint32(0);
	std::string _path = args.pop_string(1);

	create(_module_id, _path);
}

void gsf::modules::LuaScriptModule::ldispatch(uint32_t target, uint32_t event, gsf::Args args, gsf::CallbackFunc callback)
{	
	//std::cout << args.stack_index() << " " << args.get<uint32_t>(0) << " " << args.get<std::string>(1) << std::endl;
	//std::cout << target << event << args.get_count() << std::endl;
	//callback(gsf::Args(std::string("hello, callback")));

	dispatch(target, event, args, [=](gsf::Args _args) {
		try {
			callback(_args);
		}
		catch (sol::error e) {
			std::cout << e.what() << std::endl;
		}
	});
}

void gsf::modules::LuaScriptModule::llisten(uint32_t self, uint32_t event, sol::function func)
{

	listen(self, event, [=](gsf::Args args, gsf::CallbackFunc callback) {
		try {
			func(args, callback);
		}
		catch (sol::error e) {
			std::cout << e.what() << std::endl;
		}
	});

}

void gsf::modules::LuaScriptModule::create(uint32_t module_id, std::string path)
{
	LuaProxy *_lua = new LuaProxy(module_id);
	_lua->state_.open_libraries(
		sol::lib::os
		, sol::lib::base
		, sol::lib::package
		, sol::lib::math
		, sol::lib::io
		, sol::lib::table
		, sol::lib::string
		, sol::lib::debug);
	_lua->path_ = path;
	
	try
	{
		auto _ret = _lua->state_.do_file(path.c_str());
		if (_ret) {
			std::string _err = Traceback(_lua->state_.lua_state()) + " build err " 
				+ path + '\t' + lua_tostring(_lua->state_.lua_state(), _ret.stack_index()) + "\n";
			dispatch(log_module_, eid::log::error, gsf::Args(_err));
			return;
		}
	}
	catch (sol::error e)
	{
		std::string _err = Traceback(_lua->state_.lua_state()) + " init err " 
			+ path + "\t\n" + e.what();
		dispatch(log_module_, eid::log::error, gsf::Args(_err));
	}
	

	_lua->state_.new_usertype<Args>("Args"
		, "push_uint32", &Args::push_uint32
		, "push_string", &Args::push_string
		, "pop_string", &Args::pop_string
		, "pop_uint32", &Args::pop_uint32
		, "push_remote_callback", &Args::push_remote_callback
		, "pop_remote_callback", &Args::pop_remote_callback);

	_lua->state_.new_usertype<LuaScriptModule>("LuaScriptModule"
		, "ldispatch", &LuaScriptModule::ldispatch
		, "llisten", &LuaScriptModule::llisten);
	_lua->state_.set("event", this);
	_lua->state_.set("module_id", module_id);

	_lua->call_list_[LuaAppState::BEFORE_INIT] = [&](sol::table t) {
		t.get<std::function<void()>>("before_init")();
	};
	_lua->call_list_[LuaAppState::INIT] = [&](sol::table t) {
		t.get<std::function<void()>>("init")();
	};
	_lua->call_list_[LuaAppState::EXECUTE] = [&](sol::table t) {
		t.get<std::function<void()>>("execute")();
	};
	_lua->call_list_[LuaAppState::SHUT] = [&](sol::table t) {
		t.get<std::function<void()>>("shut")();
	};
	_lua->call_list_[LuaAppState::AFTER_SHUT] = [&](sol::table t) {
		t.get<std::function<void()>>("after_shut")();
	};

	try {
		sol::table _module = _lua->state_.get<sol::table>("module");
		_lua->call_list_[LuaAppState::BEFORE_INIT](_module);
		_lua->app_state_ = LuaAppState::INIT;
	}
	catch (sol::error e) {
		dispatch(log_module_, eid::log::error, gsf::Args(std::string(e.what())));
	}

	lua_map_.push_back(std::make_pair(module_id, _lua));
}

void gsf::modules::LuaScriptModule::destroy_event(gsf::Args args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_uint32(0);
	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (t.first == _module_id);
	});

	if (itr == lua_map_.end()) {
		return;
	}

	LuaProxy *_lua = itr->second;
	_lua->app_state_ = LuaAppState::AFTER_SHUT;
}

int gsf::modules::LuaScriptModule::destroy(uint32_t module_id)
{
	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (t.first == module_id);
	});

	if (itr == lua_map_.end()) {
		return -1;
	}

	LuaProxy *_lua = itr->second;

	delete _lua;
	_lua = nullptr;
	lua_map_.erase(itr);

	return 0;
}

void gsf::modules::LuaScriptModule::reload_event(gsf::Args args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_uint32(0);

	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (t.first == _module_id);
	});

	if (itr == lua_map_.end()) {
		return;
	}

	std::string _path = itr->second->path_;

	if (destroy(_module_id) != 0) {
		return;
	}

	create(_module_id, _path);
}