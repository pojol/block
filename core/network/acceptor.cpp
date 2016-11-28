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


gsf::network::Acceptor::Acceptor(const AcceptorConfig &config, std::function<void(int)> func)
	: config_(config)
	, accept_handler_(func)
{

}


gsf::network::Acceptor::~Acceptor()
{
	// ...

	close();
}


int gsf::network::Acceptor::close()
{
	evconnlistener_free(listener_ptr_);

	return 0;
}


gsf::network::AcceptorConfig & gsf::network::Acceptor::get_config()
{
	return config_;
}

void gsf::network::Acceptor::handler_new_connect(int32_t session_id)
{
	accept_handler_(session_id);
}

