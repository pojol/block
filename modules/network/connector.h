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
			void eMakeConncetor(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void eSendMsg(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

			void needCloseSession(int fd);

		private:

			uint32_t moduleID_;

			event_base *eventBasePtr_ = nullptr;

			SessionPtr sessionPtr_ = nullptr;
			MsgBinder *binderPtr_ = nullptr;

			::bufferevent *bufferEventPtr_ = nullptr;
		};
	}
}


#endif
