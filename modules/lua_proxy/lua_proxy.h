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

			void init();

			void execute();

			void shut();

		private:
			//sol::variadic_args args
			void ldispatch(uint32_t lua_id, uint32_t target, uint32_t event, gsf::Args args, gsf::CallbackFunc callback = nullptr);
			void llisten(uint32_t lua_id, uint32_t self, uint32_t event, sol::function func);

			void create_event(gsf::Args args, gsf::CallbackFunc callback);
			void create(uint32_t module_id, std::string dar_name, std::string file_name);

			void destroy_event(gsf::Args args, gsf::CallbackFunc callback);
			int destroy(uint32_t module_id);

			void reload_event(gsf::Args args, gsf::CallbackFunc callback);

		private:
			LuaProxy * find_lua(uint32_t id);

		private:
			uint32_t log_module_;

			typedef std::vector<std::pair<uint32_t, LuaProxy*>> StateMap;
			StateMap lua_map_;
		};
	}
}

#endif
