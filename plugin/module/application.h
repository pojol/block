#ifndef _GSF_APPLICATION_HEADER_
#define _GSF_APPLICATION_HEADER_

#include "module.h"


namespace gsf
{
	namespace core
	{
		class Application
		{

		public:

			void init_args();

			void regist_module(Module *module);

			void run();

			void exit();

		private:

		};
	}
}

#endif
