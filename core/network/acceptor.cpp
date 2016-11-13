#include "acceptor.h"

#include "session.h"

#include "network.h"
#include "err.h"

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include <stdint.h>

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#endif // WIN32



gsf::Acceptor::Acceptor(uint32_t id, const AcceptorConfig &config)
    :config_(config)
	,id_(id)
{

}

gsf::Acceptor::~Acceptor()
{
	// ...

	close();
}

int gsf::Acceptor::open(AcceptHandler *accept_handler)
{
	handler_ = accept_handler;

	listener_ptr_ = Network::instance().accept_bind(this, "", config_.port);
	if (listener_ptr_){
		return 0;
	}
	else {
		return LIBEVENT_NEW_BIND_ERR;
	}
}

int gsf::Acceptor::close()
{
	evconnlistener_free(listener_ptr_);

	return 0;
}

void gsf::Acceptor::handler_new_connect(int32_t session_id)
{
    handler_->handler_new_connection(id_, session_id);
}

gsf::AcceptorConfig & gsf::Acceptor::get_config()
{
	return config_;
}

uint32_t gsf::Acceptor::get_id() const
{
	return id_;
}

