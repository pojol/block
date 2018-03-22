#include "connector.h"

#include "sessionMgr.h"
#include "session.h"

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
	eventBasePtr_ = event_base_new();
	sessionMgr_ = new SessionMgr();

	using namespace std::placeholders;

	mailboxPtr_->listen(eid::network::make_connector, std::bind(&ConnectorModule::eMakeConncetor, this, _1, _2));
	mailboxPtr_->listen(eid::network::send, std::bind(&ConnectorModule::eSendMsg, this, _1, _2));
}

void gsf::network::ConnectorModule::init()
{
	// todo ...
	
	//boardcast(eid::module_init_succ, gsf::makeArgs(get_module_id()));
}

void gsf::network::ConnectorModule::execute()
{
	//! 先处理断连

	//
	sessionMgr_->exec(mailboxPtr_);

	if (eventBasePtr_) {
		event_base_loop(eventBasePtr_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}
}

void gsf::network::ConnectorModule::shut()
{
	bufferevent_free(bufferEventPtr_);
	event_base_free(eventBasePtr_);
}

void gsf::network::ConnectorModule::after_shut()
{
}

void gsf::network::ConnectorModule::eMakeConncetor(gsf::ModuleID target, gsf::ArgsPtr args)
{
	uint32_t _module_id = args->pop_i32();
	std::string _ip = args->pop_string();
	uint32_t _port = args->pop_i32();

	module_id_ = _module_id;

	int _fd = 0;

	bufferEventPtr_ = bufferevent_socket_new(eventBasePtr_, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bufferEventPtr_) {
		APP.ERR_LOG("Connector", "bufferevent socket new fail!");
		return;
	}

	struct sockaddr_in _sin;

	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);

	if (evutil_inet_pton(AF_INET, _ip.c_str(), &_sin.sin_addr) <= 0)
	{
		APP.ERR_LOG("Connector", "err_inet_pton fail!");
		return;
	}

	int _ret = bufferevent_socket_connect(bufferEventPtr_, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (_ret != 0) {
		APP.ERR_LOG("Connector", "bufferevent socket connect fail!");
		return;
	}
	else {
		_fd = bufferevent_getfd(bufferEventPtr_);
	}

	sessionPtr_ = std::make_shared<Session>(_fd, _module_id, sessionMgr_, bufferEventPtr_);
	bufferevent_setcb(bufferEventPtr_, Session::readCB, NULL, Session::eventCB, sessionPtr_.get());
	bufferevent_enable(bufferEventPtr_, EV_READ | EV_WRITE);

	APP.INFO_LOG("Connector", "make connector success!");
}


void gsf::network::ConnectorModule::needCloseSession(int fd)
{
	// 
}


void gsf::network::ConnectorModule::eSendMsg(gsf::ModuleID target, gsf::ArgsPtr args)
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

	auto _block = std::make_shared<gsf::Block>(sessionPtr_->getID(), _msg, _str);
	sessionPtr_->write(_msg, _block);
}
