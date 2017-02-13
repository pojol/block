#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

namespace gsf
{
	namespace core
	{
		class Module
		{
		protected:
			virtual void after_init();
			virtual void init();

			virtual void execute();

			virtual void before_shut();
			virtual void shut();
		};
	}
}


#endif
