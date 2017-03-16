#ifndef _GSF_CONNECTOR_HEADER_
#define _GSF_CONNECTOR_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>

#include <module/module.h>

#include <event/event.h>
#include <event2/util.h>
#include <event2/listener.h>

#include "network_event_list.h"

namespace gsf
{
	namespace network
	{
		class Session;
		typedef std::shared_ptr<Session> SessionPtr;

		class ConnectorModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			ConnectorModule();
			~ConnectorModule();

			virtual void before_init();
			virtual void init();

			virtual void execute();

			virtual void shut();
			virtual void after_shut();

		private:
			void make_connector(gsf::Args args, gsf::EventHandlerPtr callback);
			
			void need_close_session(int fd);

		private:

			uint32_t module_id_;

			event_base *event_base_ptr_;

			SessionPtr session_ptr_;
		};

	}
}


#endif
