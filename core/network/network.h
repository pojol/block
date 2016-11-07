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

	class Network
	{
	public:
		~Network();

		static Network& instance();

		int init(const NetworkConfig &config);

		int start();

        void open_acceptor(Acceptor *acceptor);

        void open_connector(ConnectorConfig &config, Connector *connector);

		void accept_conn_new(evutil_socket_t fd);

	private:
		Network();
		static Network* instance_;

		int32_t init_work_thread();

		static void worker_thread_process(evutil_socket_t fd, short event, void * arg);

		static void worker_thread_run(NetworkThread *thread);

		static void accept_listen_cb(::evconnlistener *listener
			, evutil_socket_t fd
			, sockaddr *sa
			, int socklen
			, void *arg);
	private:

		NetworkThread *main_thread_ptr_;
	
		std::vector<NetworkThread*> worker_thread_vec_;

        Acceptor *acceptor_;

		NetworkConfig config_;
	};
}


#endif
