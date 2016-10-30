#include "network.h"

#include "network_thread.h"
#include "network_config.h"


gsf::Network::Network(const NetworkConfig &config)
	: config_(config)
{
	main_thread_ptr_ = new NetworkThread();

	main_thread_ptr_->event_base_ptr_ = event_base_new();

	init_work_thread();
}

gsf::Network::~Network()
{

}

int32_t gsf::Network::init_work_thread()
{
	for (int i = 0; i < config_.m_workThreadCount; ++i)
	{
		auto threadPtr = new NetworkThread();

		threadPtr->event_base_ptr_ = event_base_new();



		worker_thread_vec_.push_back(threadPtr);
	}

	return 0;
}


void gsf::Network::worker_thread_process(int32_t fd, int16_t which, void *arg)
{

	
	//! 工作线程投递的管道消息在这里处理，并转发给主循环的主控函数。
}
