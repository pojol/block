#ifndef _GSF_NEWWORK_HEADER_
#define _GSF_NEWWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>

#include <event2/util.h>

#include "network_config.h"

namespace gsf
{
	struct NetworkThread;

    struct AcceptorConfig;
    class Acceptor;

    struct ConnectorConfig;
    class Connector;

	//! ����ģ�������
	class Network
	{
	public:
		Network(const NetworkConfig &config);

		~Network();

		int start();

        void open_acceptor(AcceptorConfig &config, Acceptor *acceptor);

        void open_connector(ConnectorConfig &config, Connector *connector);

		void accept_conn_new(evutil_socket_t fd);

	private:
		//! ��ʼ�������߳�
		int32_t init_work_thread();

		//! 接收到来自管道的消息，从队列中拿出事件处理连接逻辑。
		static void worker_thread_process(evutil_socket_t fd, short event, void * arg);

		static void worker_thread_run(NetworkThread *thread);
	private:

		NetworkThread *main_thread_ptr_;
	
		std::vector<NetworkThread*> worker_thread_vec_;

        Acceptor *acceptor_;

		const NetworkConfig config_;
	};
}


#endif
