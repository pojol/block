#ifndef _GSF_LOG_HEADER_
#define _GSF_LOG_HEADER_

#include <core/module.h>
#include <core/event.h>

#include <list>

namespace gsf
{
	namespace modules
	{
		class LogModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			LogModule();
			~LogModule() {}

			void before_init();

			void init();

			void execute();

			void shut();

		private:

			void init_impl(gsf::Args args, gsf::CallbackFunc callback);

			void log_info(gsf::Args args, gsf::CallbackFunc callback);
			void log_warning(gsf::Args args, gsf::CallbackFunc callback);
			void log_error(gsf::Args args, gsf::CallbackFunc callback);

			void log_print(uint32_t type, const char * title, gsf::Args args);
	
		private:
			std::list <std::pair<uint32_t, gsf::Args>> log_;
		};
	}
}

#endif
