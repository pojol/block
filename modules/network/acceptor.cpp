#include "acceptor.h"

#include "session_mgr.h"
#include "session.h"
#include "msg_binder.h"

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

gsf::network::AcceptorModule::AcceptorModule(const std::string &name)
	: Module(name)
{

}

gsf::network::AcceptorModule::~AcceptorModule()
{

}

void gsf::network::AcceptorModule::before_init()
{
	session_mgr_ = new SessionMgr();
	binder_ = new MsgBinder();

	event_base_ptr_ = event_base_new();

}

void gsf::network::AcceptorModule::init()
{
	listen(this, eid::network::make_acceptor
		, std::bind(&AcceptorModule::make_acceptor, this
		, std::placeholders::_1
		, std::placeholders::_2));

	listen(this, eid::network::recv_remote_callback
		, std::bind(&AcceptorModule::bind_remote, this
		, std::placeholders::_1
		, std::placeholders::_2));

	listen(this, eid::network::send_remote_callback
		, [&](const gsf::Args &args, gsf::CallbackFunc callback) {

		auto _args = gsf::Args();
		_args.push_remote_callback(std::bind(&AcceptorModule::send_msg, this
			, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		callback(_args);
	});
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
	if (session_mgr_) {
		delete session_mgr_;
		session_mgr_ = nullptr;
	}

	if (binder_) {
		delete binder_;
		binder_ = nullptr;
	}
}

void gsf::network::AcceptorModule::make_acceptor(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_int32(0);
	std::string _ip = args.pop_string(1);
	uint32_t _port = args.pop_int32(2);

	module_id_ = _module_id;	//! 绑定代理Module的id
	accept_bind(_ip, _port);
}

void gsf::network::AcceptorModule::bind_remote(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args.pop_int32(0);
	uint32_t _msg_id = args.pop_int32(1);

	auto _info_ptr = std::make_shared<RemoteInfo>(_module_id, _msg_id, callback);
	binder_->regist(_info_ptr);
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
	::bufferevent *bev;
	int32_t _ret = 0;

	do
	{
		if (network_ptr_->session_mgr_->find(fd)) {
			network_ptr_->session_mgr_->set_need_close(fd);
			_ret = eid::network::err_repeated_fd;
			break;
		}

		// check max connect
		if (network_ptr_->session_mgr_->cur_max_connet() >= NETWORK_CONNECT_MAX) {
			_ret = eid::network::err_upper_limit_session;
			break;
		}

		bev = bufferevent_socket_new(network_ptr_->event_base_ptr_, fd, BEV_OPT_CLOSE_ON_FREE);
		if (!bev) {
			_ret = eid::network::err_socket_new;
			break;
		}

	} while (0);

	if (0 == _ret) {
		auto _session_ptr = network_ptr_->session_mgr_->make_session(fd, network_ptr_->module_id_, network_ptr_->binder_);
		bufferevent_setcb(bev, Session::read_cb, NULL, Session::err_cb, _session_ptr.get());
		bufferevent_enable(bev, EV_READ | EV_WRITE);

		// dispatch event connect
		network_ptr_->dispatch(network_ptr_->module_id_, eid::network::new_connect, gsf::Args(int32_t(fd)));
	}
}

void gsf::network::AcceptorModule::send_msg(uint32_t fd, uint32_t msg_id, std::string block)
{
	auto _session_ptr = session_mgr_->find(fd);
	if (_session_ptr) {

		auto _msg = std::make_shared<gsf::Block>(fd, msg_id, block.length());
		memcpy(_msg->buf_ + _msg->pos_, block.c_str(), block.length());

		_session_ptr->write(msg_id, _msg);
	}
}

