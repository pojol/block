#include "network.h"

#include "network_thread.h"
#include "network_config.h"

gsf::Network::Network(const NetworkConfig &config)
	: config_(config)
{
	main_thread_ptr_ = std::make_shared<NetworkThread>();

	main_thread_ptr_->event_base_ptr_ = event_base_new();

	for (int i = 0; i < config.m_workThreadCount; ++i)
	{
		auto threadPtr = std::make_shared<NetworkThread>();

		worker_thread_vec_.push_back(threadPtr);

		setup_thread(threadPtr);
	}
}

int32_t gsf::Network::setup_thread(NetworkThreadPtr threadPtr)
{
	threadPtr->event_base_ptr_ = event_base_new();

	int fds[2];

	struct event *ev_receice = event_new(threadPtr->event_base_ptr_, threadPtr->notify_receive_fd_
		, EV_READ | EV_PERSIST, worker_thread_process, nullptr);
	event_add(ev_receice, nullptr);

	return 0;
}


void gsf::Network::worker_thread_process(int32_t fd, int16_t which, void *arg)
{

	
	//! 工作线程投递的管道消息在这里处理，并转发给主循环的主控函数。
}
