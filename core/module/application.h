#ifndef _GSF_APPLICATION_HEADER_
#define _GSF_APPLICATION_HEADER_

#include "module.h"

#include <list>

namespace gsf
{
	namespace core
	{
		class Application
		{

		public:

			Application();

			void init_args();

			void regist_module(Module *module);

			void run();

			void exit();

		private:

			std::list<Module *> module_list_;

			bool shutdown_;

		};
	}
}

#endif
