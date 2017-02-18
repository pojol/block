#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>

//#include <event2/event-config.h>
#include <event2/event.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif // WIN32


#include <network/network.h>
#include <network/message_binder.h>

#include <network/session.h>
#include <network/session_mgr.h>
#include <network/message.h>

#include <iostream>

#include <random>

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

static uint32_t recv_total = 0;
static uint32_t prev_total = 0;
static uint32_t cur_connect = 0;

static uint32_t old_time = 0;

class LoginServerHandler
{
public:
    ~LoginServerHandler()
	{

	}
	LoginServerHandler()
	{
		gsf::network::MessageBinder<SampleMsg>::instance().
			regist_msg_proc<LoginServerHandler, &LoginServerHandler::test_msg>(100, this);

	}

    //! new connection bind session to message dispatch
    void handler_new_connection(int session_id)
    {
		printf("new connection session_id : %d\n", session_id);

		//! bind message register
		gsf::network::Network::get_ref().regist_binder(
			&gsf::network::MessageBinder<SampleMsg>::instance());

		cur_connect++;

		//test
		SampleMsg::Ptr _ret_msg = std::make_shared<SampleMsg>();

		*_ret_msg->get_ostream() << 12;		//size
		*_ret_msg->get_ostream() << 101;	//mid
		*_ret_msg->get_ostream() << 1;

		gsf::network::Network::get_ref().write(session_id, _ret_msg);
    }

	void handler_connect_close(int session_id)
	{
		printf("dis connection session_id : %d\n", session_id);
		cur_connect--;
	}

	void test_msg(SampleMsg::Ptr msg)
	{
		//test
		uint32_t dat;
		*msg->get_istream() >> dat;

		/*
		std::cout << "session : " << msg->get_session_id()
			<< " message : " << msg->get_message_id() 
			<< " dat : " << dat << std::endl;
		*/

		//test
		SampleMsg::Ptr _ret_msg = std::make_shared<SampleMsg>();

		recv_total += 1;

		*_ret_msg->get_ostream() << 12;		//size
		*_ret_msg->get_ostream() << 101;	//mid
		*_ret_msg->get_ostream() << dat+1;

		gsf::network::Network::get_ref().write(msg->get_session_id(), _ret_msg);
	}

private:
	uint32_t index_;

};

void update()
{

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

	new Network();

	NetworkConfig _config;
	_config.buff_wait_time_ = 200;
	Network::get_ref().init(_config);

	LoginServerHandler *accept_handler = new LoginServerHandler();

	if (Network::get_ref().listen("", 8888
		, std::bind(&LoginServerHandler::handler_new_connection, accept_handler, std::placeholders::_1)
		, std::bind(&LoginServerHandler::handler_connect_close, accept_handler, std::placeholders::_1)) < 0){
		//err
	}

	Network::get_ref().start(std::bind(&update));

	Network::get_ref().uninit();
	return 0;
}
