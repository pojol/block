#include "acceptor.h"

#include "session.h"

#include "network_imp.h"
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


gsf::network::Acceptor::Acceptor(const AcceptorConfig &config, std::function<void(int)> newConnect, std::function<void(int)> disConnect)
	: config_(config)
	, new_connect_handler(newConnect)
	, dis_connect_handler(disConnect)
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

void gsf::network::Acceptor::err_cb(::bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)
	{
		/* connection has been closed, do any clean up here */
		//printf("connection closed\n");

		Session * _session_ptr = static_cast<Session *>(ctx);
		NetworkImpl::instance().get_acceptor()->handler_dis_connect(_session_ptr->get_id());
	}
	else if (what & BEV_EVENT_ERROR)
	{
		/* check errno to see what error occurred */
		Session * _session_ptr = static_cast<Session *>(ctx);
		NetworkImpl::instance().get_acceptor()->handler_dis_connect(_session_ptr->get_id());
	}
	else if (what & BEV_EVENT_TIMEOUT)
	{
		/* must be a timeout event handle, handle it */
		printf("Timed out\n");
	}
	bufferevent_free(bev);
}

gsf::network::AcceptorConfig & gsf::network::Acceptor::get_config()
{
	return config_;
}

void gsf::network::Acceptor::handler_new_connect(int32_t session_id)
{
	new_connect_handler(session_id);
}

void gsf::network::Acceptor::handler_dis_connect(int32_t session_id)
{
	dis_connect_handler(session_id);
}

