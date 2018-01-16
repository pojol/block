#ifndef _GSF_LUA_PROXY_HEADER_
#define _GSF_LUA_PROXY_HEADER_

#include <sol.hpp>
#include <array>

#include <core/module.h>
#include <core/event.h>

#include <vector>
#include <string>

namespace gsf
{
	namespace modules
	{
		enum LuaAppState
		{
			BEFORE_INIT = 0,
			INIT,
			EXECUTE,
			SHUT,
			AFTER_SHUT,
		};

		struct LuaProxy
		{
			LuaProxy(uint32_t lua_id)
				: lua_id_(lua_id)
				, app_state_(LuaAppState::BEFORE_INIT)
			{}

			sol::state state_;
			std::string dir_name_;
			std::string file_name_;
			LuaAppState app_state_;
			uint32_t lua_id_;

			std::array<std::function<void(sol::table)>, 5> call_list_;
		};

		class LuaProxyModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			LuaProxyModule()
				: Module("LuaProxyModule")
			{}

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		private:
			//sol::variadic_args args

			//代理下event的dispatch 和 listen 接口，因为要在这里集中捕获下lua产生的异常。
			std::string ldispatch(uint32_t lua_id, uint32_t target, uint32_t event, const std::string &buf);

			int llisten(uint32_t lua_id, uint32_t self, uint32_t event, const sol::function &func);

			void lrpc(uint32_t lua_id, uint32_t event, int32_t moduleid, const std::string &buf, const sol::function &func);

			gsf::ArgsPtr create_event(const gsf::ArgsPtr &args);
			void create(uint32_t module_id, std::string dar_name, std::string file_name);

			gsf::ArgsPtr destroy_event(const gsf::ArgsPtr &args);
			int destroy(uint32_t module_id);

			gsf::ArgsPtr reload_event(const gsf::ArgsPtr &args);

		private:
			LuaProxy * find_lua(uint32_t id);

		private:
			uint32_t log_m_ = 0;

			std::unordered_map<gsf::ModuleID, LuaProxy*> lua_map_;
		};
	}
}

#endif
