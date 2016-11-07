#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>

#include <event2/event-config.h>
#include <event2/event.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif // WIN32

#include <network.h>
#include <acceptor.h>
#include <accept_handler.h>

//! user interface test

class LoginServerHandler : public gsf::AcceptHandler
{
public:
    virtual ~LoginServerHandler(){}

    //! new connection bind session to message dispatch
    virtual void handler_new_connection(int acceptor_id, int session_id)
    {
        //open_session(session_id, &network::MessageHandler<>());
    }
};

/*
class Login2GateServerHandler : public ConnectHandler
{
public:
    virtual ~Login2GateServerHandler(){}

    virtual void handler_new_connection(int connector_id, int session_id){}

    virtual void handler_connect_failed(){}
};
*/


int main()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0){
		exit(1);
	}
#endif // WIN32

	gsf::NetworkConfig _config;
	_config.worker_thread_count_ = 4;
	gsf::Network::instance().init(_config);
    

    gsf::AcceptorConfig _acceptConfig;
	_acceptConfig.port = 8888;
    gsf::Acceptor _acceptor(_acceptConfig, new LoginServerHandler());

    //ConnectorConfig _connectConfig;
    //Connector _connector(_connectConfig, new Login2GateServerHandler());

    gsf::Network::instance().open_acceptor(&_acceptor);
    //NetWork::instance().open_connector(_connector);

	gsf::Network::instance().start();

	return 0;
}
