#ifndef _GSF_CONNECTOR_HEADER_
#define _GSF_CONNECTOR_HEADER_

#include <vector>
#include <memory>
#include <functional>

#include <core/module.h>
#include <core/event.h>

#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

namespace gsf
{
	namespace network
	{
		class Session;
		class MsgBinder;
		typedef std::shared_ptr<Session> SessionPtr;

		class ConnectorModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			ConnectorModule(const std::string &name);
			ConnectorModule();
			virtual ~ConnectorModule();

			void before_init() override;
			void init() override;

			void execute() override;

			void shut() override;
			void after_shut() override;

		private:
			void event_make_connector(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void event_send_msg(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

			void need_close_session(int fd);

		private:

			uint32_t module_id_;

			uint32_t log_m_;

			event_base *event_base_ptr_;

			SessionPtr session_ptr_;
			MsgBinder *binder_;

			::bufferevent *buffer_event_ptr_;

			std::vector<int> disconnect_vec_;
		};
	}
}


#endif
