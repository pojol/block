#ifndef _GSF_NETWORK_HEADER_
#define _GSF_NETWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>

#include <future>

#include <event2/util.h>
#include <event2/listener.h>

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

		typedef std::shared_ptr<Acceptor> AcceptorPtr;
		typedef std::shared_ptr<NetworkThread> NetworkThreadPtr;

		struct NetworkConfig
		{
			NetworkConfig()
			: worker_thread_count_(1)
			, send_wait_time_(20)
			, read_wait_time_(20)
			{}

			int32_t worker_thread_count_;  //! 如果不在这里指定工作线程数，则由程序自行分配当前电脑核心总数给APP
			int32_t send_wait_time_;
			int32_t read_wait_time_;
		};


		class Network
		{
			friend class Acceptor;
			friend class Connector;
		public:
			~Network();

			static Network& instance();

			int init(const NetworkConfig &config);

			int make_acceptor(const AcceptorConfig &config, AcceptHandler *accept_handler);

			AcceptorPtr get_acceptor() { return acceptor_ptr_; }

			int start();

			//temp
			std::vector<NetworkThreadPtr> & get_worker_thread() { return worker_thread_vec_; }

		protected:
			void accept_conn_new(evutil_socket_t fd);

			void accept_bind(const std::string &ip, int port);

			int connect_bind(Connector *connector_ptr, const std::string &ip, int port);

		private:
			Network();
			static Network* instance_;

			int32_t init_work_thread();

			static void worker_thread_process(evutil_socket_t fd, short event, void *arg);

			static void worker_thread_run(NetworkThreadPtr thread_ptr);

			static void accept_listen_cb(::evconnlistener *listener
				, evutil_socket_t fd
				, sockaddr *sa
				, int socklen
				, void *arg);

			static void send_wait_time_cb(evutil_socket_t fd, short event, void *arg);
			static void read_wait_time_cb(evutil_socket_t fd, short event, void *arg);
		private:

			NetworkThreadPtr main_thread_ptr_;

			std::vector<NetworkThreadPtr> worker_thread_vec_;

			AcceptorPtr acceptor_ptr_;
			::evconnlistener *accept_listener_;

			NetworkConfig config_;
		};
	}
}


#endif
