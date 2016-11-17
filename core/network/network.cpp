#include "network.h"

#include "network_thread.h"

#include "acceptor_mgr.h"
#include "session_mgr.h"
#include "acceptor.h"
#include "connector.h"
#include "session.h"
#include "err.h"

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <errno.h>
#endif // WIN32

#include <event2/listener.h>
#include <event2/bufferevent.h>

#include <thread>
#include <mutex>

static std::mutex pipe_lock;
gsf::Network* gsf::Network::instance_ = NULL;

gsf::Network::Network()
{

}

gsf::Network::~Network()
{

}

gsf::Network& gsf::Network::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::Network();
	}
	return *instance_;
}

int gsf::Network::init(const NetworkConfig &config)
{
	config_ = config;

	main_thread_ptr_ = std::make_shared<NetworkThread>();

	main_thread_ptr_->event_base_ptr_ = event_base_new();

	main_thread_ptr_->in_buffer_ = new IBuffer();

	struct ::event * time_event = nullptr;
	time_event = event_new(main_thread_ptr_->event_base_ptr_, -1, EV_PERSIST, read_wait_time_cb, main_thread_ptr_.get());

	struct timeval tv = { 0, config_.read_wait_time_ * 1000 };
	evtimer_add(time_event, &tv);
	//evtimer_del(time_event);

	init_work_thread();

	return 0;
}

int gsf::Network::start()
{
	for (auto &worker : worker_thread_vec_)
	{
		worker->th = new std::thread(worker_thread_run, worker); //非空构造会直接启动线程
	}

	event_base_dispatch(main_thread_ptr_->event_base_ptr_);

	return 0;
}

int32_t gsf::Network::init_work_thread()
{
	for (int i = 0; i < config_.worker_thread_count_; ++i)
	{
		auto thread_ptr = std::make_shared<NetworkThread>();

		thread_ptr->event_base_ptr_ = event_base_new();
		
		thread_ptr->connect_queue_ = new CQ();
		NetworkConnect::instance().cq_init(thread_ptr->connect_queue_);

		thread_ptr->out_buffer_ = new OBuffer();
		thread_ptr->in_buffer_ = new IBuffer();

		evutil_socket_t pipe[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, pipe) < 0){
			printf("evutil_socketpair err!\n");
		}
		thread_ptr->notify_send_fd_ = pipe[1];
		thread_ptr->notify_receive_fd_ = pipe[0];
		evutil_make_socket_nonblocking(thread_ptr->notify_send_fd_);
		evutil_make_socket_nonblocking(thread_ptr->notify_receive_fd_);


		struct event *signal;
		signal = event_new(thread_ptr->event_base_ptr_
			, thread_ptr->notify_receive_fd_
			, EV_READ | EV_PERSIST
			, worker_thread_process
			, thread_ptr.get());
		if (signal){
			thread_ptr->notify_event_ = signal;
			event_add(signal, NULL);
		}

		struct ::event * time_event = nullptr;
		time_event = event_new(thread_ptr->event_base_ptr_, -1, EV_PERSIST, send_wait_time_cb, thread_ptr.get());

		struct timeval tv = {0 , config_.send_wait_time_*1000};
		evtimer_add(time_event, &tv);
		//evtimer_del(time_event);

		worker_thread_vec_.push_back(thread_ptr);
	}

	return 0;
}

static int last_thread = -1;

void gsf::Network::accept_conn_new(int acceptor_id, evutil_socket_t fd)
{
	CQ_ITEM *item = NetworkConnect::instance().cqi_new();
	if (!item){
		printf("cqi_new err!\n");
		return;
	}
	
	int tid = (last_thread + 1) % config_.worker_thread_count_;
	
	auto thread_ptr = worker_thread_vec_[tid];
	
	last_thread = tid;
	
	item->sfd = fd;
	//temp
	item->acceptor_id = acceptor_id;

	NetworkConnect::instance().cq_push(thread_ptr->connect_queue_, item);

	char buf[1];
	buf[0] = 'c';
	if (send(thread_ptr->notify_send_fd_, buf, 1, 0) < 0){
		printf("pipe send err! %s\n", evutil_socket_geterror(thread_ptr->notify_send_fd_)); //
	}
}

evconnlistener * gsf::Network::accept_bind(Acceptor *acceptor_ptr, const std::string &ip, int port)
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	::evconnlistener *listener;

	listener = evconnlistener_new_bind(Network::instance().main_thread_ptr_->event_base_ptr_
		, accept_listen_cb
		, acceptor_ptr
		, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE
		, -1
		, (sockaddr*)&sin
		, sizeof(sockaddr_in));

	return listener;
}


int gsf::Network::connect_bind(Connector *connector_ptr, const std::string &ip, int port)
{
	::bufferevent *bev = bufferevent_socket_new(main_thread_ptr_->event_base_ptr_, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev){
		return LIBEVENT_BUFFER_EVENT_CONSTRUCT_ERR;
	}

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(ip.c_str());

	int fd = bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sockaddr_in));
	if (fd < 0){
		connector_ptr->handle_connect_failed(fd, ip, port);
		bufferevent_free(bev);
	}

	auto _session_ptr = SessionMgr::instance().make_session(bev, fd, nullptr, nullptr);
	bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, _session_ptr.get());
	bufferevent_enable(bev, EV_READ);

	//! connector 的消息发送绑定在主循环上，和acceptor分开处理。
	// todo

	return 0;
}


void gsf::Network::worker_thread_process(evutil_socket_t fd, short event, void * arg)
{
	//! just use point not copy
	NetworkThread *threadPtr = static_cast<NetworkThread*>(arg);

	char buf[1];
	int n = recv(fd, buf, 1, 0);
	if (n == -1){
		int err = evutil_socket_geterror(fd);
		printf("pipe recv err!\n");
	}

	switch (buf[0])
	{
	case 'c':
		CQ_ITEM *item = NetworkConnect::instance().cq_pop(threadPtr->connect_queue_);
		if (item){
			pipe_lock.lock();
			::bufferevent *bev;
			bev = bufferevent_socket_new(threadPtr->event_base_ptr_, item->sfd, BEV_OPT_CLOSE_ON_FREE);
			if (!bev){
				printf("bufferevent_socket_new err!\n");
			}

			auto _acceptor_ptr = AcceptorMgr::instance().find_acceptor(item->acceptor_id);
			if (_acceptor_ptr){
				auto _session_ptr = SessionMgr::instance().make_session(bev, item->sfd, threadPtr->out_buffer_, threadPtr->in_buffer_);
				item->session_id = _session_ptr->get_id();

				//send connection event
				_acceptor_ptr->handler_new_connect(_session_ptr->get_id());

				bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, _session_ptr.get());
				bufferevent_enable(bev, EV_READ);
			}
			else {
				printf("worker_thread_process not find acceptor!\n");
			}
			pipe_lock.unlock();
		}
		break;
	}
}


void gsf::Network::worker_thread_run(NetworkThreadPtr thread_ptr)
{
	event_base_dispatch(thread_ptr->event_base_ptr_);
}


void gsf::Network::accept_listen_cb(::evconnlistener *listener, evutil_socket_t fd, sockaddr *sa, int socklen, void *arg)
{
	Acceptor *_acceptor_ptr = static_cast<Acceptor*>(arg);

	Network::instance().accept_conn_new(_acceptor_ptr->get_id(), fd);
}


void gsf::Network::send_wait_time_cb(evutil_socket_t fd, short event, void *arg)
{
	auto _thread_ptr = static_cast<NetworkThread*>(arg);
	_thread_ptr->out_buffer_->send_evbuffer();
}

void gsf::Network::read_wait_time_cb(evutil_socket_t fd, short event, void *arg)
{
	auto _thread_ptr = static_cast<NetworkThread*>(arg);

	//swap to main_thread
	auto _threads = Network::instance().get_worker_thread();
	for (auto &th : _threads)
	{
		th->in_buffer_->lock.lock();
		_thread_ptr->in_buffer_->push_evbuffer(th->in_buffer_->active_buffer_vec_);
		th->in_buffer_->lock.unlock();
	}

	//dispatch
	_thread_ptr->in_buffer_->pop_evbuffer();
}