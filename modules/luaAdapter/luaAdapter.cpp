
#include "luaAdapter.h"

#include <core/application.h>

#include <algorithm>
#include <iostream>
#include <fmt/format.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#ifdef __cplusplus  
extern "C" {  // only need to export C interface if  
			  // used by C++ source code  
	int luaopen_protobuf_c(lua_State *L);
}
#endif  

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

void gsf::modules::LuaAdapterModule::before_init()
{	
	using namespace std::placeholders;

	mailboxPtr_->listen(eid::lua::reload, std::bind(&LuaAdapterModule::eReload, this, _1, _2));
}

void gsf::modules::LuaAdapterModule::init()
{
	create();
}

void gsf::modules::LuaAdapterModule::execute()
{
	//!
	try {
		if (reloading) {
			create();
			return;
		}

		mailboxPtr_->pull();

		sol::table _module = proxyPtr_->state_.get<sol::table>("module");

		if (proxyPtr_->app_state_ == LuaAppState::INIT) {
			proxyPtr_->call_list_[LuaAppState::INIT](_module);
			proxyPtr_->app_state_ = LuaAppState::EXECUTE;
		}
		else if (proxyPtr_->app_state_ == LuaAppState::EXECUTE) {
			proxyPtr_->call_list_[LuaAppState::EXECUTE](_module);
		}
		else if (proxyPtr_->app_state_ == LuaAppState::SHUT) {
			proxyPtr_->call_list_[LuaAppState::SHUT](_module);
			proxyPtr_->app_state_ = LuaAppState::AFTER_SHUT;
		}
		else if (proxyPtr_->app_state_ = LuaAppState::AFTER_SHUT) {
			proxyPtr_->call_list_[LuaAppState::AFTER_SHUT](_module);
		}
	}
	catch (sol::error e) {
		std::string _err = e.what() + '\n' + Traceback(proxyPtr_->state_);
		APP.ERR_LOG("LuaProxy", _err);

		if (proxyPtr_->app_state_ == LuaAppState::INIT) {
			//destroy(_lua->lua_id_);
		}
		else {

		}
	}
}

void gsf::modules::LuaAdapterModule::shut()
{
	
}

void gsf::modules::LuaAdapterModule::ldispatch(gsf::ModuleID target, gsf::EventID event, const std::string &buf)
{
	auto _smartPtr = gsf::ArgsPool::get_ref().get();

	try {
		_smartPtr->importBuf(buf);

		mailboxPtr_->dispatch(target, event, std::move(_smartPtr));
	}
	catch (sol::error e) {
		std::string _err = e.what() + '\n' + Traceback(proxyPtr_->state_);
		APP.ERR_LOG("LuaProxy", _err);
	}
	catch (...)
	{
		APP.ERR_LOG("LuaProxy", "unknown err by ldispatch", " {} {}", target, event);
	}
}

int gsf::modules::LuaAdapterModule::llisten(uint32_t event, const sol::function &func)
{
	try {

		mailboxPtr_->listen(event, [=](gsf::ModuleID target, gsf::ArgsPtr args)->void {
			try {
				std::string _req = "";
				if (args) {
					_req = args->exportBuf();
					func(_req);
				}
				else {
					func(nullptr, 0);
				}
			}
			catch (sol::error e) {
				std::string _err = e.what() + '\n' + Traceback(proxyPtr_->state_);
				APP.ERR_LOG("LuaProxy", _err);
			}
		});

	}
	catch (sol::error e) {
		std::string _err = e.what() + '\n' + Traceback(proxyPtr_->state_);
		APP.ERR_LOG("LuaProxy", _err);
	}
	catch (...)
	{
		APP.ERR_LOG("LuaProxy", "unknown err by llisten", " {} {}", getModuleID(), event);
	}
	return 0;
}

void gsf::modules::LuaAdapterModule::lrpc(uint32_t event, int32_t moduleid, const std::string &buf)
{
	/*
	auto _smartPtr = gsf::ArgsPool::get_ref().get();
	auto lua = findLua(lua_id);

	try {
		_smartPtr->push_block(buf.c_str(), buf.size());
		
		auto _callback = [=](const ArgsPtr &cbArgs, int32_t progress, bool bResult) {
			try {
				std::string _res = "";
				auto _pos = 0;
				if (nullptr != cbArgs) {
					_pos = cbArgs->get_size();
					_res = cbArgs->pop_block(0, _pos);
				}
				func(_res, _pos, progress, bResult);
			}
			catch (sol::error e) {
				std::string _err = e.what() + '\n' + Traceback(findLua(lua_id)->state_);
				APP.ERR_LOG("LuaProxy", _err);
			}
		};

		if (sol::type::lua_nil == func.get_type()) {
			//rpc(event, moduleid, std::move(_smartPtr), nullptr);
		}
		else {
			//rpc(event, moduleid, std::move(_smartPtr), _callback);
		}

	}
	catch (sol::error e)
	{
		std::string _err = e.what() + '\n' + Traceback(findLua(lua_id)->state_);
		APP.ERR_LOG("LuaProxy", _err);
	}
	catch (...)
	{
		APP.ERR_LOG("LuaProxy", "unknown err by llisten", " {} {}", moduleid, event);
	}
	*/
}

void gsf::modules::LuaAdapterModule::create()
{
	if (proxyPtr_) {	// reload
		proxyPtr_.reset(new LuaProxy);
	}
	else {
		proxyPtr_ = std::make_shared<LuaProxy>();
	}

	proxyPtr_->state_.open_libraries(
		sol::lib::os
		, sol::lib::base
		, sol::lib::package
		, sol::lib::math
		, sol::lib::io
		, sol::lib::table
		, sol::lib::string
		, sol::lib::debug);
	proxyPtr_->dir_name_ = dir_;
	proxyPtr_->file_name_ = name_;

	auto _path = dir_ + "/" + name_;

	try
	{
		// set pbc to lua global
		//luaopen_protobuf_c(proxyPtr_->state_.lua_state());

		proxyPtr_->state_.require("protobuf.c", luaopen_protobuf_c);

		auto _ret = proxyPtr_->state_.do_file(_path.c_str());
		if (_ret) {
			std::string _err = Traceback(proxyPtr_->state_.lua_state()) + " build err "
				+ _path + '\t' + lua_tostring(proxyPtr_->state_.lua_state(), _ret.stack_index()) + "\n";

			APP.ERR_LOG("LuaProxy", _err);
			return;
		}

	}
	catch (sol::error e)
	{
		std::string _err = e.what() + '\n';
		_err += Traceback(proxyPtr_->state_.lua_state());

		APP.ERR_LOG("LuaProxy", _err);
	}


	proxyPtr_->state_.new_usertype<gsf::Args>("Args"
		, sol::constructors<Args(), Args(const char *, int)>()
		, "push_ui16", &Args::push_ui16
		, "push_ui32", &Args::push_ui32
		, "push_i32", &Args::push_i32
		, "push_string", &Args::push_string
		, "push_bool", &Args::push_bool
		, "pop_string", &Args::pop_string
		, "pop_ui16", &Args::pop_ui16
		, "pop_ui32", &Args::pop_ui32
		, "pop_i32", &Args::pop_i32
		, "pop_i64", &Args::pop_i64
		, "pop_ui64", &Args::pop_ui64
		, "pop_bool", &Args::pop_bool
		, "get_size", &Args::get_size
		, "get_tag", &Args::get_tag
		, "get_offset", &Args::get_offset
		, "importBuf", &Args::importBuf
		, "exportBuf", &Args::exportBuf);

	proxyPtr_->state_.new_usertype<LuaAdapterModule>("LuaProxyModule"
		, "ldispatch", &LuaAdapterModule::ldispatch
		, "llisten", &LuaAdapterModule::llisten
		, "lrpc", &LuaAdapterModule::lrpc);
	proxyPtr_->state_.set("event", this);
	proxyPtr_->state_.set("module_id", getModuleID());

	proxyPtr_->state_.new_usertype<gsf::Application>("Application"
		, "getModule", &Application::getModule
		, "getAppName", &Application::getAppName
		, "getMachine", &Application::getMachine
		, "getUUID", &Application::getUUID);
	proxyPtr_->state_.set("APP", gsf::Application::get_ptr());

	proxyPtr_->call_list_[LuaAppState::BEFORE_INIT] = [&](sol::table t) {
		t.get<std::function<void(std::string)>>("before_init")(dir_ + "/");
	};
	proxyPtr_->call_list_[LuaAppState::INIT] = [&](sol::table t) {
		t.get<std::function<void()>>("init")();
	};
	proxyPtr_->call_list_[LuaAppState::EXECUTE] = [&](sol::table t) {
		t.get<std::function<void()>>("execute")();
	};
	proxyPtr_->call_list_[LuaAppState::SHUT] = [&](sol::table t) {
		t.get<std::function<void()>>("shut")();
	};
	proxyPtr_->call_list_[LuaAppState::AFTER_SHUT] = [&](sol::table t) {
		t.get<std::function<void()>>("after_shut")();
	};

	try {
		sol::table _module = proxyPtr_->state_.get<sol::table>("module");
		proxyPtr_->call_list_[LuaAppState::BEFORE_INIT](_module);
		proxyPtr_->app_state_ = LuaAppState::INIT;
	}
	catch (sol::error e) {
		std::string _err = e.what() + '\n';
		_err += Traceback(proxyPtr_->state_.lua_state());
		APP.ERR_LOG("LuaProxy", _err);

		return;
	}

	reloading = false;
}

int gsf::modules::LuaAdapterModule::destroy(uint32_t module_id)
{

	return 0;
}

void gsf::modules::LuaAdapterModule::eReload(gsf::ModuleID target, gsf::ArgsPtr args)
{
	try {

		sol::table _module = proxyPtr_->state_.get<sol::table>("module");
		proxyPtr_->call_list_[LuaAppState::SHUT](_module);

		reloading = true;
	}
	catch (sol::error e) {
		std::string _err = e.what() + '\n';
		_err += Traceback(proxyPtr_->state_.lua_state());
		APP.ERR_LOG("LuaProxy", _err);
	}
	catch (...) {
		APP.ERR_LOG("LuaProxy", "unknown");
	}
}