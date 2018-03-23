#ifndef _GSF_LOG_HEADER_
#define _GSF_LOG_HEADER_

#include <core/module.h>

#include <list>

namespace gsf
{
	namespace modules
	{
		class LogModule
			: public gsf::Module
		{
		public:
			LogModule();
			~LogModule() {}

			void before_init() override;

			void init() override;

			void execute() override;

			void shut() override;

		private:

			void initImpl(const std::string &exe_name);

			void ePrint(gsf::ModuleID target, gsf::ArgsPtr args);
			void eChangeFlag(gsf::ModuleID target, gsf::ArgsPtr args);

		private:
			bool ndebug_ = false;
			char path_[512];
		};
	}
}

#endif
