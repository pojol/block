#include "tcpConnector.h"

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

block::network::TcpConnectorModule::TcpConnectorModule(const std::string &name)
	: Module(name)
{
}

block::network::TcpConnectorModule::TcpConnectorModule()
	: Module("ConnectorModule")
{

}

block::network::TcpConnectorModule::~TcpConnectorModule()
{

}

void block::network::TcpConnectorModule::before_init()
{
	eventBasePtr_ = event_base_new();
	sessionMgr_ = new SessionMgr(this);

	using namespace std::placeholders;

	listen(block::event::tcp_make_connector, std::bind(&TcpConnectorModule::eMakeConncetor, this, _1, _2));
	listen(block::event::tcp_send, std::bind(&TcpConnectorModule::eSendMsg, this, _1, _2));
}

void block::network::TcpConnectorModule::init()
{
	// todo ...
	
	boardcast(block::event::module_init_succ, block::makeArgs(getModuleID(), getModuleName()));
}

void block::network::TcpConnectorModule::execute()
{
	//! 先处理断连

	//
	sessionMgr_->exec();

	if (eventBasePtr_) {
		event_base_loop(eventBasePtr_, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}
}

void block::network::TcpConnectorModule::shut()
{
	bufferevent_free(bufferEventPtr_);
	event_base_free(eventBasePtr_);
}

void block::network::TcpConnectorModule::after_shut()
{
}

void block::network::TcpConnectorModule::eMakeConncetor(block::ModuleID target, block::ArgsPtr args)
{
	std::string _ip = args->pop_string();
	uint32_t _port = args->pop_i32();

	proxyModuleID_ = target;

	int _fd = 0;

	bufferEventPtr_ = bufferevent_socket_new(eventBasePtr_, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bufferEventPtr_) {
		ERROR_LOG("[BLOCK] tcpConnector : bufferevent socket new fail!");
		return;
	}

	struct sockaddr_in _sin;

	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);

	if (evutil_inet_pton(AF_INET, _ip.c_str(), &_sin.sin_addr) <= 0)
	{
		ERROR_LOG("[BLOCK] tcpConnector : err_inet_pton fail!");
		return;
	}

	int _ret = bufferevent_socket_connect(bufferEventPtr_, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (_ret != 0) {
		ERROR_LOG("[BLOCK] tcpConnector : bufferevent socket connect fail!");
		return;
	}
	else {
		_fd = bufferevent_getfd(bufferEventPtr_);
	}

	//sessionPtr_ = std::make_shared<Session>(_fd, target, sessionMgr_, bufferEventPtr_);
	sessionPtr_ = sessionMgr_->makeSession(_fd, target, bufferEventPtr_);
	bufferevent_setcb(bufferEventPtr_, Session::readCB, NULL, Session::eventCB, sessionPtr_.get());
	bufferevent_enable(bufferEventPtr_, EV_READ | EV_WRITE);

	INFO_LOG("[BLOCK] tcpConnector : make connector success!");
}


void block::network::TcpConnectorModule::needCloseSession(int fd)
{
	// 
}


void block::network::TcpConnectorModule::eSendMsg(block::ModuleID target, block::ArgsPtr args)
{
	auto _fd = args->pop_fd();
	auto _msg = args->pop_msgid();
	std::string _str = args->pop_string();

	auto _block = std::make_shared<block::Block>(sessionPtr_->getID(), _msg, _str);
	sessionPtr_->write(_msg, _block);
}
