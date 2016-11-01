#include "network.h"

#include "network_thread.h"
#include "network_config.h"

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <netinet/in.h>
#endif // WIN32


#include <thread>

#include <event2/listener.h>

static void listener_event_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *arg);

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

int gsf::Network::start()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0){
		exit(1);
	}
#endif // WIN32

    struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(config_.port_);


	struct evconnlistener *listener;
	listener = evconnlistener_new_bind(main_thread_ptr_->event_base_ptr_
		, listener_event_cb
		, (void*)this
		, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE
		, -1
		, (sockaddr*)&sin
		, sizeof(sockaddr_in));
	if (NULL == listener){

	}

	for (auto &worker : worker_thread_vec_)
	{
		std::thread wt(worker_thread_run, worker);
		wt.join();
	}

	event_base_dispatch(main_thread_ptr_->event_base_ptr_);

	return 0;
}

int32_t gsf::Network::init_work_thread()
{
	for (int i = 0; i < config_.worker_thread_count_; ++i)
	{
		auto threadPtr = new NetworkThread();

		threadPtr->event_base_ptr_ = event_base_new();

		evutil_socket_t pipe[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, pipe) < 0){

		}
		threadPtr->notify_send_fd_ = pipe[1];
		threadPtr->notify_receive_fd_ = pipe[0];
		evutil_make_socket_nonblocking(threadPtr->notify_send_fd_);
		evutil_make_socket_nonblocking(threadPtr->notify_receive_fd_);


		struct event *signal;
		signal = event_new(threadPtr->event_base_ptr_
			, threadPtr->notify_receive_fd_
			, EV_READ | EV_PERSIST
			, worker_thread_process
			, threadPtr);
		if (signal){
			threadPtr->notify_event_ = signal;
			event_add(signal, NULL);
		}

		worker_thread_vec_.push_back(threadPtr);
	}

	return 0;
}


void gsf::Network::dispatch_conn_new(evutil_socket_t fd)
{


	//int tid = (last_thread + 1) % config_.worker_thread_count_;

	// ��ʼ��session

	char buf[1];
	buf[0] = 'c';
	send(worker_thread_vec_[0]->notify_send_fd_, buf, 1, 0);
}

void gsf::Network::worker_thread_process(evutil_socket_t fd, short event, void * arg)
{
	NetworkThread *threadPtr = static_cast<NetworkThread*>(arg);

	char buf[1];
	int n = recv(fd, buf, 1, 0);
	if (n == -1){
		int err = evutil_socket_geterror(fd);
	}

	switch (buf[0])
	{
	case 'c':
		break;
	}
}

void gsf::Network::worker_thread_run(NetworkThread *thread)
{
	event_base_dispatch(thread->event_base_ptr_);
}

void listener_event_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *arg)
{
	gsf::Network *network = static_cast<gsf::Network*>(arg);

	//test
	network->dispatch_conn_new(fd);
}