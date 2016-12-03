#ifndef _NETWORK_CONFIG_HEADER_
#define _NETWORK_CONFIG_HEADER_

#include <stdint.h>

namespace gsf
{
	namespace network
	{
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
	}
}

#endif
