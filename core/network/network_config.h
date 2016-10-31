#ifndef _GSF_NEWWORK_CONFIG_HEADER_
#define _GSF_NEWWORK_CONFIG_HEADER_

#include <stdint.h>

namespace gsf
{
	//! 处理网络层配置相关类
	struct NetworkConfig
	{
		int32_t worker_thread_count_;		//网络工作线程数量

		int32_t port_;


	};
}


#endif
