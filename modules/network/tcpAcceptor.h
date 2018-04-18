#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <vector>
#include <memory>
#include <functional>

#include <core/module.h>

#include <event2/util.h>
#include <event2/listener.h>

namespace gsf
{
	namespace network
	{
		class SessionMgr;

		class TcpAcceptorModule
			: public gsf::Module
		{
		public:
			TcpAcceptorModule(const std::string &name);
			TcpAcceptorModule();
			virtual ~TcpAcceptorModule();

			void before_init() override;
			void init() override;

			void execute() override;

			void shut() override;
			void after_shut() override;

		private:

			void eMakeAcceptor(gsf::ModuleID target, gsf::ArgsPtr args);

			void eSendMsg(gsf::ModuleID target, gsf::ArgsPtr args);

			void eKick(gsf::ModuleID target, gsf::ArgsPtr args);

			void accept_bind(const std::string &ip, int port);
			static void accept_listen_cb(::evconnlistener *listener
				, evutil_socket_t fd
				, sockaddr *sa
				, int socklen
				, void *arg);

		private:

			uint32_t moduleID_;

			SessionMgr *sessionMgr_ = nullptr;

			event_base *eventBasePtr_ = nullptr;

			::evconnlistener *acceptListenerPtr_ = nullptr;
		};

	}
}


#endif
