#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif // WIN32

#include <module/application.h>
#include <event/event.h>

#include <network/network.h>

#include <iostream>

#include <random>

static int test_tick = -1;

class TestNetworkApp : public gsf::core::Application
{
public:
	void tick()
	{
		int t = (test_tick + 1) % 50;

		if (t == 0){
			//printf("delay %d\n", delay_);
		}

		test_tick = t;
	}

};

class TestNetworkModule
	: public gsf::core::Module
	, public gsf::core::Door
{
public:

	void init()
	{
		listen_callback(event_id::network::new_connect, [=](gsf::Args args){
		});

		listen_callback(event_id::network::dis_connect, [=](gsf::Args args){
		});

		// 
		gsf::Args _a_args;
		_a_args << get_door_id() << std::string("127.0.0.1") << uint32_t(8001);

		dispatch(event_id::network::make_acceptor, _a_args);

		//
		gsf::Args _s_args;
		dispatch(event_id::network::start_network, _s_args);
	}

};

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


	TestNetworkApp app;
	new gsf::core::EventModule;

	app.regist_module(gsf::core::EventModule::get_ptr());
	app.regist_module(new gsf::network::NetworkModule);
	app.regist_module(new TestNetworkModule);

	app.run();

	return 0;
}
