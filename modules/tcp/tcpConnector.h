#ifndef _GSF_CONNECTOR_HEADER_
#define _GSF_CONNECTOR_HEADER_

#include <vector>
#include <memory>
#include <functional>

#include <core/module.h>

#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

namespace gsf
{
	namespace network
	{
		class Session;
		class SessionMgr;

		typedef std::shared_ptr<Session> SessionPtr;

		class TcpConnectorModule
			: public gsf::Module
		{
		public:
			TcpConnectorModule(const std::string &name);
			TcpConnectorModule();
			virtual ~TcpConnectorModule();

			void before_init() override;
			void init() override;

			void execute() override;

			void shut() override;
			void after_shut() override;

		private:
			void eMakeConncetor(gsf::ModuleID target, gsf::ArgsPtr args);
			void eSendMsg(gsf::ModuleID target, gsf::ArgsPtr args);

			void needCloseSession(int fd);

		private:

			uint32_t moduleID_;

			event_base *eventBasePtr_ = nullptr;

			SessionPtr sessionPtr_ = nullptr;
			SessionMgr *sessionMgr_ = nullptr;

			::bufferevent *bufferEventPtr_ = nullptr;
		};
	}
}


#endif
