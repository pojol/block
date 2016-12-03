#ifndef _NETWORK_IMPL_HEADER_
#define _NETWORK_IMPL_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>

#include <event2/util.h>
#include <event2/listener.h>

#include "network_config.h"

namespace gsf
{
	namespace network
	{
		struct NetworkThread;

		struct AcceptorConfig;
		class Acceptor;
		class AcceptHandler;

		struct ConnectorConfig;
		class Connector;

		struct NetworkConfig;

		class NetworkImpl
		{
			typedef std::shared_ptr<Acceptor> AcceptorPtr;
			typedef std::shared_ptr<NetworkThread> NetworkThreadPtr;

			friend class Acceptor;
			friend class Connector;

		public:
			~NetworkImpl();
			static NetworkImpl& instance();

			int init(const NetworkConfig &config);

			int start();

			int make_acceptor(const AcceptorConfig &config, std::function<void(int)> func);

			void write(uint32_t session_id, const char *data, uint32_t len);

		protected:
			NetworkImpl();
			static NetworkImpl* instance_;

			std::vector<NetworkThreadPtr> & get_worker_thread() { return worker_thread_vec_; }
			NetworkThreadPtr get_main_thread() { return main_thread_ptr_; }

			AcceptorPtr get_acceptor() { return acceptor_ptr_; }

		protected:
			void accept_conn_new(evutil_socket_t fd);

			void accept_bind(const std::string &ip, int port);

			int connect_bind(Connector *connector_ptr, const std::string &ip, int port);

			int32_t init_work_thread();

			static void worker_thread_process(evutil_socket_t fd, short event, void *arg);

			static void worker_thread_run(NetworkThreadPtr thread_ptr);

			static void accept_listen_cb(::evconnlistener *listener
				, evutil_socket_t fd
				, sockaddr *sa
				, int socklen
				, void *arg);

			static void main_produce_event(evutil_socket_t fd, short event, void *arg);
			static void main_consume_event(evutil_socket_t fd, short event, void *arg);

			static void work_produce_event(evutil_socket_t fd, short event, void *arg);
			static void work_consume_event(evutil_socket_t fd, short event, void *arg);

		private:
			NetworkConfig config_;

			NetworkThreadPtr main_thread_ptr_;

			std::vector<NetworkThreadPtr> worker_thread_vec_;

			AcceptorPtr acceptor_ptr_;
			::evconnlistener *accept_listener_;

			::event * produce_event_;
			::event * consume_event_;

			::event * work_produce_event_;
			::event * work_consume_event_;
		};
	}
}

#endif
