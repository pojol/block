#ifndef _GSF_NETWORK_HEADER_
#define _GSF_NETWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>

#include <event2/util.h>
#include <event2/listener.h>

#include "network_config.h"

namespace gsf
{
	struct NetworkThread;

    struct AcceptorConfig;
    class Acceptor;

    struct ConnectorConfig;
    class Connector;

	typedef std::shared_ptr<Acceptor> AcceptorPtr;
	typedef std::shared_ptr<NetworkThread> NetworkThreadPtr;

	class Network
	{
	public:
		~Network();

		static Network& instance();

		int init(const NetworkConfig &config);

		int start();

		void open_acceptor(AcceptorPtr acceptor_ptr);

        void open_connector(ConnectorConfig &config, Connector *connector);

	protected:
		void accept_conn_new(evutil_socket_t fd);

	private:
		Network();
		static Network* instance_;

		int32_t init_work_thread();

		static void worker_thread_process(evutil_socket_t fd, short event, void * arg);

		static void worker_thread_run(NetworkThreadPtr thread_ptr);

		static void accept_listen_cb(::evconnlistener *listener
			, evutil_socket_t fd
			, sockaddr *sa
			, int socklen
			, void *arg);
	private:

		NetworkThreadPtr main_thread_ptr_;
	
		std::vector<NetworkThreadPtr> worker_thread_vec_;

		AcceptorPtr acceptor_ptr_;

		NetworkConfig config_;
	};
}


#endif
