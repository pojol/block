#ifndef _GSF_MAILBOX_HEADER_
#define _GSF_MAILBOX_HEADER_

#include <module/module.h>
#include <event/event.h>

namespace gsf
{
	namespace modules
	{
		class Mailbox
			: public gsf::core::Module
			, public gsf::core::Door
		{
		public:
			Mailbox();
			~Mailbox();

			virtual void before_init();
			virtual void init();

			virtual void execute();

			virtual void shut();
			virtual void after_shut();
		};
	}
}

#endif
