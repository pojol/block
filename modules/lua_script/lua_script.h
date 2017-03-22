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

			void create_impl(gsf::Args args, gsf::EventHandlerPtr callback);

			void destroy_impl(gsf::Args args, gsf::EventHandlerPtr callback);

			void reload_impl(gsf::Args args, gsf::EventHandlerPtr callback);

		private:

			std::vector<std::pair<uint32_t, lua_State*>> lua_map_;
		};
	}
}

#endif
