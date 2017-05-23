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

			void init_impl(const std::string &exe_name);


			void log_print(uint32_t type, const char * title, const gsf::Args &args);
	
		private:
			
			char path_[512];
		};
	}
}

#endif
