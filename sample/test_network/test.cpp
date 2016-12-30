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

static uint32_t recv_total = 0;
static uint32_t prev_total = 0;
static uint32_t cur_connect = 0;

static uint32_t old_time = 0;

class LoginServerHandler
{
public:
    ~LoginServerHandler()
	{
		if (time_event_){
			gsf::utils::Timer::instance().rmv_timer(time_event_);
		}
	}
	LoginServerHandler()
	{
		gsf::network::MessageBinder<SampleMsg>::instance().
			regist_msg_proc<LoginServerHandler, &LoginServerHandler::test_msg>(100, this);

		using namespace gsf::utils;
		time_event_ = Timer::instance().add_timer(delay_milliseconds(1000)
			, makeTimerHandler(&LoginServerHandler::tick, this));
	}

    //! new connection bind session to message dispatch
    void handler_new_connection(int session_id)
    {
		printf("new connection session_id : %d\n", session_id);

		//! bind message register
		gsf::network::Network::instance().regist_binder(
			&gsf::network::MessageBinder<SampleMsg>::instance());

		cur_connect++;

		//test
		SampleMsg::Ptr _ret_msg = std::make_shared<SampleMsg>();

		*_ret_msg->get_ostream() << 12;		//size
		*_ret_msg->get_ostream() << 101;	//mid
		*_ret_msg->get_ostream() << 1;

		gsf::network::Network::instance().write(session_id, _ret_msg);
    }

	void handler_connect_close(int session_id)
	{
		printf("dis connection session_id : %d\n", session_id);
		cur_connect--;
	}

	void tick()
	{
		printf("total:%d\n", recv_total);
		printf("connect:%d\n", cur_connect);
		printf("tick:%d\n", recv_total - prev_total);

		prev_total = recv_total;

		using namespace std::chrono;
		auto _t = time_point_cast<milliseconds>(system_clock::now());
		printf("delay:%d\n", _t.time_since_epoch().count() - old_time);
		old_time = _t.time_since_epoch().count();

		using namespace gsf::utils;
		time_event_ = Timer::instance().add_timer(delay_milliseconds(1000)
			, makeTimerHandler(&LoginServerHandler::tick, this));
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

		gsf::network::Network::instance().write(msg->get_session_id(), _ret_msg);
	}

private:
	uint32_t index_;

	gsf::utils::TimerEvent *time_event_;
};

void update()
{
	//gettime
	
	//todo...
	gsf::utils::Timer::instance().update();

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
	_config.buff_wait_time_ = 200;
	Network::instance().init(_config);

	LoginServerHandler *accept_handler = new LoginServerHandler();

	if (Network::instance().listen("", 8888
		, std::bind(&LoginServerHandler::handler_new_connection, accept_handler, std::placeholders::_1)
		, std::bind(&LoginServerHandler::handler_connect_close, accept_handler, std::placeholders::_1)) < 0){
		//err
	}

	gsf::network::Network::instance().start(std::bind(&update));

	gsf::network::Network::instance().uninit();
	return 0;
}
