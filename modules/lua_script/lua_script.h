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
			void destroy(uint32_t module_id);

			void reload_event(gsf::Args args, gsf::EventHandlerPtr callback);

		private:

			typedef std::vector<std::pair<uint32_t, lua_State*>> StateMap;
			StateMap lua_map_;
		};
	}
}

#endif
