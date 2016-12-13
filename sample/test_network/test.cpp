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
#include <network_config.h>
#include <acceptor.h>
#include <message_binder.h>

#include <session.h>
#include <session_mgr.h>
#include <message.h>

#include <iostream>

#include <random>
#include <timer.h>
#include <timer_handler.h>

//! 由用户定制msg
class SampleMsg : public gsf::network::Message
{
public:
	typedef std::shared_ptr<SampleMsg> Ptr;

	SampleMsg()
		: gsf::network::Message()
	{}

	SampleMsg(gsf::stream::BlockPtr blockPtr, uint32_t start, uint32_t len, uint32_t session_id)
		: gsf::network::Message(blockPtr, start, len, session_id)
	{

	}

	virtual ~SampleMsg()
	{

	}

};

#include <chrono>
#include <ctime>

class LoginServerHandler
{
public:
    ~LoginServerHandler(){}
	LoginServerHandler()
	{
		gsf::network::MessageBinder<SampleMsg>::instance().
			regist_msg_proc<LoginServerHandler, &LoginServerHandler::test_msg>(100, this);
	}

    //! new connection bind session to message dispatch
    void handler_new_connection(int session_id)
    {
		printf("new connection session_id : %d\n", session_id);
		session_id_ = session_id;

		//! bind message register
		gsf::network::Network::instance().regist_binder(
			&gsf::network::MessageBinder<SampleMsg>::instance());

		//test
		SampleMsg::Ptr _ret_msg = std::make_shared<SampleMsg>();

		*_ret_msg->get_ostream() << 12;		//size
		*_ret_msg->get_ostream() << 101;	//mid
		*_ret_msg->get_ostream() << 1;

		gsf::network::Network::instance().write(session_id_, _ret_msg);
    }

	void test_msg(SampleMsg::Ptr msg)
	{
		//test
		uint32_t dat;
		*msg->get_istream() >> dat;

		std::cout << "session : " << msg->get_session_id()
			<< " message : " << msg->get_message_id() 
			<< " dat : " << dat << std::endl;

		//test
		SampleMsg::Ptr _ret_msg = std::make_shared<SampleMsg>();

		*_ret_msg->get_ostream() << 12;		//size
		*_ret_msg->get_ostream() << 101;	//mid
		*_ret_msg->get_ostream() << dat+1;

		gsf::network::Network::instance().write(session_id_, _ret_msg);
	}

private:
	uint32_t session_id_;
	uint32_t index_;
};


enum NetWorkState
{
	NS_Fast,
	NS_Slow,
	NS_Busy,
};

void update()
{
	//gettime

	//todo...

	//gettime
	//endtime - begintime = network state
	//if network state change : printf("info")
}

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

	using namespace gsf::network;

	NetworkConfig _config;
	Network::instance().init(_config);
    
	AcceptorConfig _acceptConfig;
	_acceptConfig.port = 8888;

	LoginServerHandler *accept_handler = new LoginServerHandler();

	if (Network::instance().listen(_acceptConfig
		, std::bind(&LoginServerHandler::handler_new_connection, accept_handler, std::placeholders::_1)) < 0){
		//err
	}

	gsf::network::Network::instance().start(std::bind(&update));

	return 0;
}
