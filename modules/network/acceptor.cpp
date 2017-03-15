#include "acceptor.h"

#include "session_mgr.h"
#include "session.h"

#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

gsf::network::AcceptorModule::AcceptorModule()
{

}

gsf::network::AcceptorModule::~AcceptorModule()
{

}

void gsf::network::AcceptorModule::before_init()
{
	session_mgr_ = new SessionMgr();

	event_base_ptr_ = event_base_new();

}

void gsf::network::AcceptorModule::init()
{
	listen(this, event_id::network::make_acceptor
		, std::bind(&AcceptorModule::make_acceptor, this
		, std::placeholders::_1
		, std::placeholders::_2));
}

void gsf::network::AcceptorModule::execute()
{
	if (session_mgr_) {
		session_mgr_->close();
	}

	if (event_base_ptr_) {
		event_base_loop(event_base_ptr_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}
}

void gsf::network::AcceptorModule::shut()
{
	evconnlistener_free(accept_listener_);
}

void gsf::network::AcceptorModule::after_shut()
{

}

void gsf::network::AcceptorModule::make_acceptor(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _door = args.pop_uint32(0);
	std::string _ip = args.pop_string(1);
	uint32_t _port = args.pop_uint32(2);

	door_id_ = _door;
	accept_bind(_ip, _port);
}

void gsf::network::AcceptorModule::accept_bind(const std::string &ip, int port)
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	accept_listener_ = evconnlistener_new_bind(event_base_ptr_
		, accept_listen_cb
		, (void*)this
		, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE
		, -1
		, (sockaddr*)&sin
		, sizeof(sockaddr_in));

	if (!accept_listener_) {
		printf("accept listen err!\n");
	}
}

void gsf::network::AcceptorModule::accept_listen_cb(::evconnlistener *listener, evutil_socket_t fd, sockaddr *sa, int socklen, void *arg)
{
	gsf::network::AcceptorModule *network_ptr_ = (gsf::network::AcceptorModule*)arg;

	if (network_ptr_->session_mgr_->find(fd)) {
		network_ptr_->session_mgr_->set_need_close(fd);
		printf("repeated fd %d\n", fd);
		return;
	}

	// check max connect
	if (network_ptr_->session_mgr_->cur_max_connet() >= SESSION_MAX_CONNECT) {
		printf("upper limit session!\n");
		return;
	}

	::bufferevent *bev;
	bev = bufferevent_socket_new(network_ptr_->event_base_ptr_, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		printf("bufferevent_socket_new err!\n");
		return;
	}

	auto _session_ptr = network_ptr_->session_mgr_->make_session(bev, fd, network_ptr_->door_id_);
	bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, _session_ptr.get());
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	// dispatch event connect
	gsf::Args args;
	args << uint32_t(fd);
	network_ptr_->dispatch(network_ptr_->door_id_, event_id::network::new_connect, args);
}

