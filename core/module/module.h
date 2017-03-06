#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

namespace gsf
{
	class Module
	{
		friend class Application;
	protected:
		virtual void before_init();
		virtual void init();

		virtual void execute();

		virtual void shut();
		virtual void after_shut();
	};
}


#endif
