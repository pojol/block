#ifndef _GSF_LOG_HEADER_
#define _GSF_LOG_HEADER_

#include <module/module.h>
#include <event/event.h>

#include <list>

namespace gsf
{
	namespace modules
	{
		class LogModule
			: public gsf::Module
			, public gsf::IEvent
		{
			void init();

			void execute();

			void shut();

		private:

			void init_impl(gsf::Args args, gsf::EventHandlerPtr callback);

			void log_info(gsf::Args args, gsf::EventHandlerPtr callback);
			void log_warning(gsf::Args args, gsf::EventHandlerPtr callback);
			void log_error(gsf::Args args, gsf::EventHandlerPtr callback);

		private:
			std::list <std::pair<uint32_t, gsf::Args>> log_;
		};
	}
}

#endif
