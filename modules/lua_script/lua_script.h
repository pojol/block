#ifndef _GSF_LUA_SCRIPT_HEADER_
#define _GSF_LUA_SCRIPT_HEADER_

#include <sol.hpp>

#include <module/module.h>
#include <event/event.h>

#include <vector>
#include <string>

namespace gsf
{
	namespace modules
	{
		// sol 的state是个unique_ptr， 在这里有点尴尬先这样写实现功能。
		struct LuaState
		{
			sol::state state_;
		};

		class LuaScriptModule
			: public gsf::Module
			, public gsf::IEvent
		{
			void init();

			void execute();

			void shut();

		private:
			//! 待实现，需要一个有效的方式包装或者让脚本直接调用dispatch
			void ldispatch(sol::variadic_args args);

			//! 创建一个新的lua module
			void create_event(gsf::Args args, gsf::EventHandlerPtr callback);
			void create(uint32_t module_id, std::string path);

			//! 销毁一个现有的lua module
			void destroy_event(gsf::Args args, gsf::EventHandlerPtr callback);
			int destroy(uint32_t module_id);

			//! 重载一个现有的lua module
			void reload_event(gsf::Args args, gsf::EventHandlerPtr callback);

		private:
			uint32_t log_module_;

			typedef std::vector<std::tuple<uint32_t, LuaState*, std::string>> StateMap;
			StateMap lua_map_;
		};
	}
}

#endif
