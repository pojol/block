#include "connector.h"

#include "session_mgr.h"
#include "session.h"
#include "msg_binder.h"

#include <event2/buffer.h>
#include <event2/listener.h>

#include <core/application.h>

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

gsf::network::ConnectorModule::ConnectorModule(const std::string &name)
	: Module(name)
{
}

gsf::network::ConnectorModule::ConnectorModule()
	: Module("ConnectorModule")
{

}

gsf::network::ConnectorModule::~ConnectorModule()
{

}

void gsf::network::ConnectorModule::before_init()
{
	binder_ = new MsgBinder();

	event_base_ptr_ = event_base_new();

	listen(this, eid::network::make_connector
		, std::bind(&ConnectorModule::event_make_connector, this
			, std::placeholders::_1, std::placeholders::_2));

	listen(this, eid::network::send
		, std::bind(&ConnectorModule::event_send_msg, this
			, std::placeholders::_1, std::placeholders::_2));
	

	log_m_ = APP.get_module("LogModule");
	assert(log_m_ != gsf::ModuleNil);
}

void gsf::network::ConnectorModule::init()
{
	// todo ...
	
	boardcast(eid::module_init_succ, gsf::make_args(get_module_id()));
}

void gsf::network::ConnectorModule::execute()
{
	//! 先处理断连

	if (event_base_ptr_) {
		event_base_loop(event_base_ptr_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}
}

void gsf::network::ConnectorModule::shut()
{
	wipeout(this);

	bufferevent_free(buffer_event_ptr_);
	event_base_free(event_base_ptr_);
}

void gsf::network::ConnectorModule::after_shut()
{
	if (binder_) {
		delete binder_;
		binder_ = nullptr;
	}

	boardcast(eid::module_shut_succ, gsf::make_args(get_module_id()));
}

void gsf::network::ConnectorModule::event_make_connector(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	uint32_t _module_id = args->pop_i32();
	std::string _ip = args->pop_string();
	uint32_t _port = args->pop_i32();

	module_id_ = _module_id;

	int32_t _ret = 0;
	int _fd = 0;

	do {
		buffer_event_ptr_ = bufferevent_socket_new(event_base_ptr_, -1, BEV_OPT_CLOSE_ON_FREE);
		if (!buffer_event_ptr_) {
			_ret = eid::error::err_socket_new;
			break;
		}

		struct sockaddr_in _sin;

		memset(&_sin, 0, sizeof(_sin));
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(_port);

		if (evutil_inet_pton(AF_INET, _ip.c_str(), &_sin.sin_addr) <= 0)
		{
			_ret = eid::error::err_inet_pton;
			break;
		}

		int _ret = bufferevent_socket_connect(buffer_event_ptr_, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (_ret != 0) {
			_ret = eid::error::err_socket_connect;
			break;
		}
		else {
			_fd = bufferevent_getfd(buffer_event_ptr_);
		}
	} while (0);


	if (_ret == 0) {
		session_ptr_ = std::make_shared<Session>(_fd, _module_id, binder_, std::bind(&ConnectorModule::need_close_session, this, std::placeholders::_1));
		bufferevent_setcb(buffer_event_ptr_, Session::read_cb, NULL, Session::event_cb, session_ptr_.get());
		bufferevent_enable(buffer_event_ptr_, EV_READ | EV_WRITE);

		session_ptr_->set_log_module(log_m_);
	}
}


/*
void gsf::network::ConnectorModule::bind_remote(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	uint32_t _module_id = args->pop_i32();
	uint32_t _msg_id = args->pop_i32();

	auto _info_ptr = std::make_shared<RemoteInfo>(_module_id, _msg_id, callback);
	binder_->regist(_info_ptr);
}
*/


void gsf::network::ConnectorModule::need_close_session(int fd)
{
	// 
	disconnect_vec_.push_back(fd);
}


void gsf::network::ConnectorModule::event_send_msg(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	auto _msg = args->pop_msgid();
	std::string _str = "";

	//! 内部消息走的时Args流， 外部的是原始的二进制数据。 所以这里要分开处理下!
	if (_msg > eid::distributed::rpc_begin && _msg < eid::distributed::rpc_end) {
		auto _headlen = sizeof(gsf::MsgID) + 1;
		_str = args->pop_block(_headlen, args->get_size() - _headlen);
	}
	else {
		_str = args->pop_string();
	}

	auto _block = std::make_shared<gsf::Block>(session_ptr_->get_id(), _msg, _str);
	session_ptr_->write(_msg, _block);
}
