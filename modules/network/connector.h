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
#include <event2/bufferevent.h>

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
			ConnectorModule(const std::string &name);
			ConnectorModule();
			~ConnectorModule();

			virtual void before_init();
			virtual void init();

			virtual void execute();

			virtual void shut();
			virtual void after_shut();

		private:
			void make_connector(gsf::Args args, gsf::CallbackFunc callback);
			
			void need_close_session(int fd);

			void send_msg(std::vector<uint32_t> fd_vec, uint32_t msg_id, BlockPtr blockptr);

		private:

			uint32_t module_id_;

			uint32_t log_module_;

			event_base *event_base_ptr_;

			SessionPtr session_ptr_;

			::bufferevent *buffer_event_ptr_;

			std::vector<int> disconnect_vec_;
		};
	}
}


#endif
