
#include "lua_proxy.h"

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

void gsf::modules::LuaProxyModule::before_init()
{
	dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("LogModule")), [=](const gsf::Args &args)
	{
		log_m_ = args.pop_int32(0);
	});
}

void gsf::modules::LuaProxyModule::init()
{
	listen(this, eid::lua_proxy::create
		, std::bind(&LuaProxyModule::create_event, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, eid::lua_proxy::destroy
		, std::bind(&LuaProxyModule::destroy_event, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, eid::lua_proxy::reload
		, std::bind(&LuaProxyModule::reload_event, this
			, std::placeholders::_1
			, std::placeholders::_2));

	dispatch(log_m_, eid::log::log_callback, gsf::Args(), [&](const gsf::Args &args) {
		log_f_ = args.pop_log_callback(0);
	});
}

void gsf::modules::LuaProxyModule::execute()
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
			std::string _err = e.what() + '\n';
			_err += Traceback(_lua->state_.lua_state());

			log_f_(eid::log::error, "LuaProxy", gsf::Args(_err));
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

void gsf::modules::LuaProxyModule::shut()
{
	for (auto itr : lua_map_)
	{
		uint32_t _module_id = std::get<0>(itr);
		destroy(_module_id);
	}

	lua_map_.clear();
}

void gsf::modules::LuaProxyModule::create_event(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_int32(0);
	std::string _dir_name = args.pop_string(1);
	std::string _file_name = args.pop_string(2);

	create(_module_id, _dir_name, _file_name);
}

void gsf::modules::LuaProxyModule::ldispatch(uint32_t lua_id, uint32_t target, uint32_t event, const gsf::Args &args, gsf::CallbackFunc callback)
{	
	dispatch(target, event, args, [=](const gsf::Args &_args) {
		try {
			callback(_args);
		}
		catch (sol::error e) {
	
			std::string _err = e.what() + '\n';
				
			auto lua = find_lua(lua_id);
			if (lua) {
				_err += Traceback(lua->state_.lua_state());
			}

			log_f_(eid::log::error, "LuaProxy", gsf::Args(_err));
		}
	});
}

void gsf::modules::LuaProxyModule::llisten(uint32_t lua_id, uint32_t self, uint32_t event, sol::function func)
{
	listen(self, event, [=](const gsf::Args &args, gsf::CallbackFunc callback) {
		try {
			func(args, callback);
		}
		catch (sol::error e) {
			std::string _err = e.what() + '\n';

			auto lua = find_lua(lua_id);
			if (lua) {
				_err += Traceback(lua->state_.lua_state());
			}

			log_f_(eid::log::error, "LuaProxy", gsf::Args(_err));
		}
	});
}

void gsf::modules::LuaProxyModule::create(uint32_t module_id, std::string dir_name, std::string file_name)
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
	_lua->dir_name_ = dir_name;
	_lua->file_name_ = file_name;

	auto _path = dir_name + "/script/" + file_name;

	try
	{
		auto _ret = _lua->state_.do_file(_path.c_str());
		if (_ret) {
			std::string _err = Traceback(_lua->state_.lua_state()) + " build err " 
				+ _path + '\t' + lua_tostring(_lua->state_.lua_state(), _ret.stack_index()) + "\n";
			log_f_(eid::log::error, "LuaProxy", gsf::Args(_err));
			return;
		}
	}
	catch (sol::error e)
	{
		std::string _err = e.what() + '\n';
		_err += Traceback(_lua->state_.lua_state());

		log_f_(eid::log::error, "LuaProxy", gsf::Args(_err));
	}

	_lua->state_.new_usertype<gsf::Args>("Args"
		, "push_uint32", &Args::push_uint32
		, "push_string", &Args::push_string
		, "pop_string", &Args::pop_string
		, "pop_uint32", &Args::pop_uint32
		, "pop_uint64", &Args::pop_uint64
		, "push_remote_callback", &Args::push_remote_callback
		, "pop_remote_callback", &Args::pop_remote_callback
		, "pop_log_callback", &Args::pop_log_callback);

	_lua->state_.new_usertype<LuaProxyModule>("LuaProxyModule"
		, "ldispatch", &LuaProxyModule::ldispatch
		, "llisten", &LuaProxyModule::llisten);
	_lua->state_.set("event", this);
	_lua->state_.set("module_id", module_id);

	_lua->call_list_[LuaAppState::BEFORE_INIT] = [&](sol::table t) {
		t.get<std::function<void(std::string)>>("before_init")(dir_name + "/script/");
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
		std::string _err = e.what() + '\n';
		_err += Traceback(_lua->state_.lua_state());

		log_f_(eid::log::error, "LuaProxy", gsf::Args(_err));
	}

	lua_map_.push_back(std::make_pair(module_id, _lua));
}

void gsf::modules::LuaProxyModule::destroy_event(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_int32(0);
	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (t.first == _module_id);
	});

	if (itr == lua_map_.end()) {
		return;
	}

	LuaProxy *_lua = itr->second;
	_lua->app_state_ = LuaAppState::AFTER_SHUT;
}

int gsf::modules::LuaProxyModule::destroy(uint32_t module_id)
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

void gsf::modules::LuaProxyModule::reload_event(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_int32(0);

	LuaProxy *lua = find_lua(_module_id);

	std::string _dir_name = lua->dir_name_;
	std::string _file_name = lua->file_name_;

	if (destroy(_module_id) != 0) {
		return;
	}

	create(_module_id, _dir_name, _file_name);
}

gsf::modules::LuaProxy * gsf::modules::LuaProxyModule::find_lua(uint32_t id)
{
	auto itr = std::find_if(lua_map_.begin(), lua_map_.end(), [&](StateMap::value_type t) {
		return (t.first == id);
	});

	if (itr == lua_map_.end()) {
		return nullptr;
	}
	else {
		return itr->second;
	}
}