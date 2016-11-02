#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <event2/event-config.h>
#include <event2/event.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif // WIN32


//! user interface test
/*
class LoginServerHandler : public AcceptHandler
{
public:
    virtual ~LoginServerHandler(){}

    //! new connection bind session to message dispatch
    virtual void handler_new_connection(int acceptor_id, int session_id)
    {
        open_session(session_id, &network::MessageHandler<>());
    }
};

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

    /*
    AcceptorConfig _acceptConfig;
    Acceptor _acceptor(_acceptConfig, new LoginServerHandler());
    _acceptor.open();

    ConnectorConfig _connectConfig;
    Connector _connector(_connectConfig, new Login2GateServerHandler());
    _connector.open();
    */
	return 0;
}
