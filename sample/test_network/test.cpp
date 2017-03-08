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

#include <network/acceptor.h>
#include <network/connector.h>

#include <iostream>

#include <random>

static int test_tick = -1;

class TestNetworkApp 
	: public gsf::Application
	, public gsf::utils::Singleton<TestNetworkApp>
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

class Client2LoginModule
	: public gsf::network::AcceptorModule
{
public:

private:

};

class Client2LoginProxy
	: public gsf::Module
	, public gsf::Door
{
public:
	void init()
	{
		uint32_t _c2l_id = TestNetworkApp::get_ref().find_module_id<Client2LoginModule>();

		listen(make_event(event_id::network::new_connect)
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
		});

		listen(make_event(event_id::network::dis_connect)
			, [=](gsf::Args args, gsf::EventHandlerPtr callback){
			std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_module_id() << std::string("127.0.0.1") << uint32_t(8001);
		dispatch(make_event(_c2l_id, event_id::network::make_acceptor), args);
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

	new TestNetworkApp;
	new gsf::EventModule;

	TestNetworkApp::get_ref().regist_module(gsf::EventModule::get_ptr());
	TestNetworkApp::get_ref().regist_module(new Client2LoginModule);
	TestNetworkApp::get_ref().regist_module(new Client2LoginProxy);

	TestNetworkApp::get_ref().run();

	return 0;
}
