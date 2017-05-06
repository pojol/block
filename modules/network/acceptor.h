#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>

#include <module/module.h>

#include <event/event.h>
#include <event2/util.h>
#include <event2/listener.h>

namespace gsf
{
	namespace network
	{
		class SessionMgr;

		class AcceptorModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			AcceptorModule(const std::string &name);
			~AcceptorModule();

			virtual void before_init();
			virtual void init();

			virtual void execute();

			virtual void shut();
			virtual void after_shut();

		private:

			void make_acceptor(gsf::Args args, gsf::CallbackFunc callback);
			void accept_bind(const std::string &ip, int port);
			static void accept_listen_cb(::evconnlistener *listener
				, evutil_socket_t fd
				, sockaddr *sa
				, int socklen
				, void *arg);

			void send_msg(uint32_t fd, uint32_t msg_id, BlockPtr blockptr);

		private:

			uint32_t module_id_;

			SessionMgr *session_mgr_;

			event_base *event_base_ptr_;

			::evconnlistener *accept_listener_;
		};

	}
}


#endif
