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
		public:
			LuaScriptModule()
				: Module("LuaScriptModule")
			{}

			void init();

			void execute();

			void shut();

		private:
			//! 待实现，需要一个有效的方式包装或者让脚本直接调用dispatch
			//sol::variadic_args args
			void ldispatch(uint32_t target, uint32_t event, gsf::Args args, gsf::CallbackFunc callback = nullptr);
			void ldispatch_remote(uint32_t target, uint32_t fd, uint32_t msg_id, const std::string &str);
			void llisten(uint32_t self, uint32_t event, sol::function func);

			//! 创建一个新的lua module
			void create_event(gsf::Args args, gsf::CallbackFunc callback);
			void create(uint32_t module_id, std::string path);

			//! 销毁一个现有的lua module
			void destroy_event(gsf::Args args, gsf::CallbackFunc callback);
			int destroy(uint32_t module_id);

			//! 重载一个现有的lua module
			void reload_event(gsf::Args args, gsf::CallbackFunc callback);

		private:
			uint32_t log_module_;

			typedef std::vector<std::tuple<uint32_t, LuaState*, std::string>> StateMap;
			StateMap lua_map_;
		};
	}
}

#endif
