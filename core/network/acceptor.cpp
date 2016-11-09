#include "acceptor.h"

#include "session.h"

#include "accept_handler.h"
#include "network.h"

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



gsf::Acceptor::Acceptor(AcceptorConfig &config, AcceptHandler *handler)
    :config_(config)
    ,handler_(handler)
{

}


gsf::Acceptor::~Acceptor()
{

}

static int32_t session_index = 0;

gsf::Session * gsf::Acceptor::make_session(::bufferevent *bev, int fd)
{
    Session *session = new Session(session_index++, bev, fd);

    auto itr = session_queue_.find(session->getid());
    if (itr == session_queue_.end()){
        session_queue_.insert(std::make_pair(session->getid(), session));
    }
    else {
        return nullptr;
    }

	return session;
}

void gsf::Acceptor::handler_new_connect(int32_t session_id)
{
    handler_->handler_new_connection(id_, session_id);
}

gsf::AcceptorConfig & gsf::Acceptor::get_config()
{
	return config_;
}
