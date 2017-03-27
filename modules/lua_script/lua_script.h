#ifndef _GSF_LUA_SCRIPT_HEADER_
#define _GSF_LUA_SCRIPT_HEADER_

#include <module/module.h>
#include <event/event.h>

extern "C"{
	#include <lauxlib.h>
	#include <lua.h>
	#include <lualib.h>
}

#include <vector>
#include <string>

namespace gsf
{
	namespace modules
	{

		class LuaScriptModule
			: public gsf::Module
			, public gsf::IEvent
		{
			void init();

			void execute();

			void shut();

		private:

			void create_event(gsf::Args args, gsf::EventHandlerPtr callback);
			void create(uint32_t module_id, std::string path);

			void destroy_event(gsf::Args args, gsf::EventHandlerPtr callback);
			int destroy(uint32_t module_id);

			void reload_event(gsf::Args args, gsf::EventHandlerPtr callback);

			int pcall(lua_State *L, const char * func);

		private:
			uint32_t log_module_;

			typedef std::vector<std::tuple<uint32_t, lua_State*, std::string>> StateMap;
			StateMap lua_map_;
		};
	}
}

#endif
