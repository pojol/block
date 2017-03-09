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
		uint32_t _c2l_id = AppRef.find_module_id<Client2LoginModule>();

		listen(this, event_id::network::new_connect
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
		});

		listen(this, event_id::network::dis_connect
			, [=](gsf::Args args, gsf::EventHandlerPtr callback){
			std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_module_id() << std::string("127.0.0.1") << uint32_t(8001);
		dispatch(_c2l_id, event_id::network::make_acceptor, args);
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

	new gsf::Application;
	new gsf::EventModule;

	AppRef.regist_module(gsf::EventModule::get_ptr());
	AppRef.regist_module(new Client2LoginModule);
	AppRef.regist_module(new Client2LoginProxy);

	AppRef.run();

	return 0;
}
