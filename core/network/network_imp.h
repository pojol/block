﻿#ifndef _NETWORK_IMPL_HEADER_
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

		struct AcceptorConfig;
		class Acceptor;
		class AcceptHandler;

		struct ConnectorConfig;
		class Connector;
		class Binder;
		class Message;
		typedef std::shared_ptr<Message> MessagePtr;

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

			int start(std::function<void()> update_func);

			int make_acceptor(const AcceptorConfig &config, std::function<void(int)> newConnect, std::function<void(int)> disConnect);

			int make_connector(const ConnectorConfig &config, std::function<void(int)> newConnect, std::function<void(int, int, std::string&, int)> connFailed);

			void write(uint32_t session_id, MessagePtr msg);

			void regist_binder(Binder *binder);

		protected:
			NetworkImpl();
			static NetworkImpl* instance_;

			std::vector<NetworkThreadPtr> & get_worker_thread() { return worker_thread_vec_; }
			NetworkThreadPtr get_main_thread() { return main_thread_ptr_; }

			AcceptorPtr get_acceptor() { return acceptor_ptr_; }

			Binder *get_binder() const { return binder_; }

			std::function<void()> get_update_func() { return update_func_; }

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

			static void main_thread_event(evutil_socket_t fd, short event, void *arg);
			static void work_thread_event(evutil_socket_t fd, short event, void *arg);

			static void update_event(evutil_socket_t fd, short event, void *arg);

		private:
			NetworkConfig config_;

			NetworkThreadPtr main_thread_ptr_;

			std::vector<NetworkThreadPtr> worker_thread_vec_;

			AcceptorPtr acceptor_ptr_;
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
