#ifndef _GSF_LUA_SCRIPT_HEADER_
#define _GSF_LUA_SCRIPT_HEADER_

#include <module/module.h>
#include <event/event.h>

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

		};
	}
}

#endif
