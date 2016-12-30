#ifndef _NETWORK_IMPL_HEADER_
#define _NETWORK_IMPL_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>

#include <event2/util.h>
#include <event2/listener.h>

#include "network.h"
#include "message_binder.h"

namespace gsf
{
	namespace network
	{
		struct NetworkThread;

		class Binder;
		class Message;
		typedef std::shared_ptr<Message> MessagePtr;



		class NetworkImpl
		{
			typedef std::shared_ptr<NetworkThread> NetworkThreadPtr;

			friend class Acceptor;
			friend class Connector;

		public:
			~NetworkImpl();
			static NetworkImpl& instance();

			int init(const NetworkConfig &config);
			void uninit();

			int start(std::function<void()> update_func);

			int make_acceptor(const std::string &ip, uint32_t port, NewConnectFunc newConnect, DisConnectFunc disConnect);
			int make_connector(const std::string &ip, uint32_t port, NewConnectFunc newConnect, ConnectFailedFunc connFailed);

			void write(uint32_t session_id, MessagePtr msg);

			void regist_binder(Binder *binder);

			NewConnectFunc newconnect_func;
			DisConnectFunc disconnect_func;
			ConnectFailedFunc failconnect_func;

		protected:
			NetworkImpl();
			static NetworkImpl* instance_;

			std::vector<NetworkThreadPtr> & get_worker_thread() { return worker_thread_vec_; }
			NetworkThreadPtr get_main_thread() { return main_thread_ptr_; }

			Binder *get_binder() const { return binder_; }

			std::function<void()> get_update_func() { return update_func_; }

		protected:
			void acceptor_conn_new(evutil_socket_t fd);
			void connector_conn_new(const std::string &ip, uint32_t port);

			void accept_bind(const std::string &ip, int port);

			int32_t init_work_thread();

			static void worker_thread_process(evutil_socket_t fd, short event, void *arg);

			static void worker_thread_run(NetworkThreadPtr thread_ptr);

			static void accept_listen_cb(::evconnlistener *listener
				, evutil_socket_t fd
				, sockaddr *sa
				, int socklen
				, void *arg);

			static void main_thread_event(evutil_socket_t fd, short event, void *arg);
			static void work_thread_event(evutil_socket_t fd, short event, void *arg);

			static void update_event(evutil_socket_t fd, short event, void *arg);

		private:
			NetworkConfig config_;

			NetworkThreadPtr main_thread_ptr_;

			std::vector<NetworkThreadPtr> worker_thread_vec_;

			::evconnlistener *accept_listener_;

			::event * main_thread_event_;
			::event * work_thread_event_;

			::event * update_event_;
			std::function<void()> update_func_;

			Binder *binder_;
		};
	}
}

#endif
