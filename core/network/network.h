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

	//! 网络模块核心类
	class Network
	{
	public:
		Network(const NetworkConfig &config);

		~Network();

		int start();

		void dispatch_conn_new(evutil_socket_t fd);
	private:
		//! 初始化工作线程
		int32_t init_work_thread();

		static void worker_thread_process(evutil_socket_t fd, short event, void * arg);

		static void worker_thread_run(NetworkThread *thread);

	private:

		NetworkThread *main_thread_ptr_;
	
		std::vector<NetworkThread*> worker_thread_vec_;

		const NetworkConfig config_;
	};
}


#endif
