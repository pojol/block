#include "acceptor.h"

#include "accept_handler.h"
#include "network.h"

#include <event2/listener.h>
#include <event2/bufferevent.h>

#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#endif // WIN32



gsf::Acceptor::Acceptor(AcceptorConfig &config, AcceptHandler *handler)
    :config_(config)
    ,handler_(handler)
{

}


gsf::Session * gsf::Acceptor::make_session()
{
	return nullptr;
}

void gsf::Acceptor::hander_new_connect()
{
    handler_->handler_new_connection(1, 1);
}