#ifndef _GSF_NETWORK_HEADER_
#define _GSF_NETWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>

#include <module/module.h>
#include <event/event.h>

#include "network_event_list.h"

namespace gsf
{
	namespace network
	{
		class NetworkModule
			: public gsf::core::Module
			, public gsf::core::Door
		{
		public:
			NetworkModule();
			~NetworkModule();

			virtual void before_init();
			virtual void init();

			virtual void execute();

			virtual void shut();
			virtual void after_shut();

		private:

			void start(gsf::Args args, gsf::core::EventHandlerPtr callback);

			void make_acceptor(gsf::Args args, gsf::core::EventHandlerPtr callback);

		};

	}
}


#endif
