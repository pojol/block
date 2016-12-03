#include "network_imp.h"

#include "network_thread.h"

#include "session_mgr.h"
#include "acceptor.h"
#include "connector.h"
#include "session.h"
#include "err.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#endif // WIN32

#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include <thread>
#include <mutex>

//! debug
#include <iostream>

gsf::network::NetworkImpl* gsf::network::NetworkImpl::instance_ = NULL;


gsf::network::NetworkImpl::NetworkImpl()
	: produce_event_(nullptr)
	, consume_event_(nullptr)
	, work_produce_event_(nullptr)
	, work_consume_event_(nullptr)
{

}


gsf::network::NetworkImpl& gsf::network::NetworkImpl::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::network::NetworkImpl();
	}
	return *instance_;
}


gsf::network::NetworkImpl::~NetworkImpl()
{
	evtimer_del(produce_event_);
	evtimer_del(consume_event_);
}

int gsf::network::NetworkImpl::init(const NetworkConfig &config)
{
	config_ = config;

	main_thread_ptr_ = std::make_shared<NetworkThread>(0);

	main_thread_ptr_->event_base_ptr_ = event_base_new();

	produce_event_ = event_new(main_thread_ptr_->event_base_ptr_, -1, EV_PERSIST, main_produce_event, main_thread_ptr_.get());
	consume_event_ = event_new(main_thread_ptr_->event_base_ptr_, -1, EV_PERSIST, main_consume_event, main_thread_ptr_.get());

	struct timeval tv = { 0, config_.read_wait_time_ * 1000 };
	evtimer_add(produce_event_, &tv);
	evtimer_add(consume_event_, &tv);

	init_work_thread();

	return 0;
}

int gsf::network::NetworkImpl::start()
{
	for (auto &worker : worker_thread_vec_)
	{
		worker->th = new std::thread(worker_thread_run, worker); //非空构造会直接启动线程
	}

	event_base_dispatch(main_thread_ptr_->event_base_ptr_);

	return 0;
}

int32_t gsf::network::NetworkImpl::init_work_thread()
{
	if (config_.worker_thread_count_ == 1){
		config_.worker_thread_count_ = std::thread::hardware_concurrency() - 1;
	}
	for (int i = 0; i < config_.worker_thread_count_; ++i)
	{
		auto thread_ptr = std::make_shared<NetworkThread>(i + 1);

		thread_ptr->event_base_ptr_ = event_base_new();

		thread_ptr->connect_queue_ = new CQ();
		NetworkConnect::instance().cq_init(thread_ptr->connect_queue_);

		evutil_socket_t pipe[2];
#ifdef WIN32
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, pipe) < 0){
#else
		if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pipe) < 0){
#endif
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

		work_produce_event_ = event_new(thread_ptr->event_base_ptr_, -1, EV_PERSIST, work_produce_event, thread_ptr.get());
		work_consume_event_ = event_new(thread_ptr->event_base_ptr_, -1, EV_PERSIST, work_consume_event, thread_ptr.get());

		struct timeval tv = { 0, config_.send_wait_time_ * 1000 };
		evtimer_add(work_produce_event_, &tv);
		evtimer_add(work_consume_event_, &tv);

		worker_thread_vec_.push_back(thread_ptr);
	}

	main_thread_ptr_->out_buffer_->mian_thread_init(config_.worker_thread_count_ + 1);

	return 0;
}

static int last_thread = -1;

void gsf::network::NetworkImpl::accept_conn_new(evutil_socket_t fd)
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

	NetworkConnect::instance().cq_push(thread_ptr->connect_queue_, item);

	char buf[1];
	buf[0] = 'c';
	if (send(thread_ptr->notify_send_fd_, buf, 1, 0) < 0){
		printf("pipe send err! %d\n", evutil_socket_geterror(thread_ptr->notify_send_fd_)); //
	}
}

void gsf::network::NetworkImpl::accept_bind(const std::string &ip, int port)
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	accept_listener_ = evconnlistener_new_bind(NetworkImpl::instance().main_thread_ptr_->event_base_ptr_
		, accept_listen_cb
		, acceptor_ptr_.get()
		, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE
		, -1
		, (sockaddr*)&sin
		, sizeof(sockaddr_in));

	if (!accept_listener_){
		printf("accept listen err!\n");
	}
}

int gsf::network::NetworkImpl::connect_bind(Connector *connector_ptr, const std::string &ip, int port)
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

	auto _session_ptr = main_thread_ptr_->session_mgr->make_session(bev, fd);
	bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, _session_ptr.get());
	bufferevent_enable(bev, EV_READ);

	return 0;
}

void gsf::network::NetworkImpl::worker_thread_process(evutil_socket_t fd, short event, void *arg)
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

			::bufferevent *bev;
			bev = bufferevent_socket_new(threadPtr->event_base_ptr_, item->sfd, BEV_OPT_CLOSE_ON_FREE);
			if (!bev){
				printf("bufferevent_socket_new err!\n");
			}

			auto _session_ptr = threadPtr->session_mgr->make_session(bev, item->sfd);
			_session_ptr->init(threadPtr->in_buffer_, threadPtr->out_buffer_);
			item->session_id = _session_ptr->get_id();

			threadPtr->in_buffer_->new_connect(_session_ptr->get_id());

			bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, _session_ptr.get());
			bufferevent_enable(bev, EV_READ);
		}
		break;
	}
}

int gsf::network::NetworkImpl::make_acceptor(const AcceptorConfig &config, std::function<void(int)> func)
{
	acceptor_ptr_ = std::make_shared<Acceptor>(config, func);

	accept_bind(config.address, config.port);

	return 0;
}

void gsf::network::NetworkImpl::worker_thread_run(NetworkThreadPtr thread_ptr)
{
	event_base_dispatch(thread_ptr->event_base_ptr_);
}

void gsf::network::NetworkImpl::accept_listen_cb(::evconnlistener *listener, evutil_socket_t fd, sockaddr *sa, int socklen, void *arg)
{
	Acceptor *_acceptor_ptr = static_cast<Acceptor*>(arg);

	NetworkImpl::instance().accept_conn_new(fd);
}

void gsf::network::NetworkImpl::main_produce_event(evutil_socket_t fd, short event, void *arg)
{
	auto *_thread_ptr = static_cast<NetworkThread*>(arg);

	_thread_ptr->out_buffer_->produce();
}

void gsf::network::NetworkImpl::main_consume_event(evutil_socket_t fd, short event, void *arg)
{
	auto *_thread_ptr = static_cast<NetworkThread*>(arg);

	for (auto &th : NetworkImpl::instance().get_worker_thread())
	{
		std::vector<std::pair<uint32_t, evbuffer*>> vec;
		std::vector<uint32_t> conn;
		th->in_buffer_->consume(vec, conn);

		// test dispatch
		for (auto &it : vec)
		{
			int len = evbuffer_get_length(it.second);
			char * buf = (char *)malloc(len);
			evbuffer_remove(it.second, buf, len);

			std::cout << "thread : " << std::this_thread::get_id() << " session : " << it.first << " recv : " << buf << std::endl;

			evbuffer_free(it.second);
		}

		vec.clear();

		for (int i : conn)
		{
			NetworkImpl::instance().get_acceptor()->handler_new_connect(i);
		}
	}
}

void gsf::network::NetworkImpl::work_produce_event(evutil_socket_t fd, short event, void *arg)
{
	auto *_thread_ptr = static_cast<NetworkThread*>(arg);

	_thread_ptr->in_buffer_->produce();
}

void gsf::network::NetworkImpl::work_consume_event(evutil_socket_t fd, short event, void *arg)
{
	auto *_thread_ptr = static_cast<NetworkThread*>(arg);

	auto _main_thread_ptr = NetworkImpl::instance().get_main_thread();

	std::vector<std::pair<uint32_t, evbuffer*>> vec;
	_main_thread_ptr->out_buffer_->consume(_thread_ptr->index_, vec);

	for (auto &it : vec)
	{
		SessionPtr _seesion = _thread_ptr->session_mgr->find(it.first);
		_seesion->write(it.second);
	}

	vec.clear();
}

void gsf::network::NetworkImpl::write(uint32_t session_id, const char *data, uint32_t len)
{
	main_thread_ptr_->out_buffer_->write(session_id, data, len);
}

