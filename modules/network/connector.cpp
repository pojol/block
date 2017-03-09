#include "connector.h"

#include "session_mgr.h"
#include "session.h"

#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>


gsf::network::ConnectorModule::ConnectorModule()
{

}

gsf::network::ConnectorModule::~ConnectorModule()
{

}

void gsf::network::ConnectorModule::before_init()
{
	new SessionMgr();

	event_base_ptr_ = event_base_new();

}

void gsf::network::ConnectorModule::init()
{
	listen(this, event_id::network::make_connector
		, std::bind(&ConnectorModule::make_connector, this
		, std::placeholders::_1
		, std::placeholders::_2));
}

void gsf::network::ConnectorModule::execute()
{
	if (event_base_ptr_) {
		event_base_loop(event_base_ptr_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}
}

void gsf::network::ConnectorModule::shut()
{

}

void gsf::network::ConnectorModule::after_shut()
{

}

void gsf::network::ConnectorModule::make_connector(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _door = args.pop_uint32(0);
	std::string _ip = args.pop_string(1);
	uint32_t _port = args.pop_uint32(2);

	door_id_ = _door;

	::bufferevent *_bev_ptr = bufferevent_socket_new(event_base_ptr_, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!_bev_ptr) {
		printf("bufferevent_socket_new err!\n");
		return;
	}

	struct sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);
	_sin.sin_addr.s_addr = inet_addr(_ip.c_str());

	int _fd = bufferevent_socket_connect(_bev_ptr, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (_fd < 0) {
		printf("bufferevent_socket_connect err!\n");
		return;
	}

	session_ptr_ = std::make_shared<Session>(_bev_ptr, _fd);
	bufferevent_setcb(_bev_ptr, Session::read_cb, NULL, Session::err_cb, session_ptr_.get());
	bufferevent_enable(_bev_ptr, EV_READ | EV_WRITE);

	gsf::Args res;
	res << uint32_t(_fd);
	dispatch(door_id_, event_id::network::new_connect, res);
}