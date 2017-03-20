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

#include <log/log.h>

#include <iostream>

#include <random>

#include "test.pb.h"

class Client2LoginModule
	: public gsf::network::AcceptorModule
{
public:

private:

};

class EntityMgr
	: public gsf::Module
	, public gsf::IEvent
{
public:

	void init()
	{
		//test
		gsf::Args cfg;
		cfg << std::string("network_test") << std::string("E:\\github\\gsf\\build\\bin");
		dispatch(AppRef.find_module_id<gsf::modules::LogModule>(), event_id::log::init, cfg);

		gsf::Args args;
		args << std::string("hello, log!");
		dispatch(AppRef.find_module_id<gsf::modules::LogModule>(), event_id::log::info, args);

		uint32_t _em_id = AppRef.find_module_id<gsf::EventModule>();

		auto arr = {
			std::make_pair(uint32_t(1001), std::bind(&EntityMgr::test_remote, this, std::placeholders::_1, std::placeholders::_2)),
		};

		for (auto nod : arr)
		{
			//! 向协议绑定器申请，module 和 协议的绑定.
			gsf::Args args;
			args << get_module_id() << nod.first << nod.second;
			dispatch(_em_id, event_id::network::bind_remote_callback, args);
		}
	}

	void test_remote(uint32_t fd, gsf::BlockPtr blockptr)
	{
		test_network::Info _info;
		_info.ParseFromArray(blockptr->buf_, blockptr->size_);
		std::cout << _info.id() << " " << _info.name() << std::endl;

		_info.set_name("world");

		AppRef.sendmsg<Client2LoginModule>(this, fd, 1002, _info);
	}

};

class Client2LoginProxy
	: public gsf::Module
	, public gsf::IEvent
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

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	new gsf::Application;
	new gsf::EventModule;

	AppRef.regist_module(gsf::EventModule::get_ptr());
	AppRef.regist_module(new gsf::modules::LogModule);
	AppRef.regist_module(new Client2LoginModule);
	AppRef.regist_module(new Client2LoginProxy);
	AppRef.regist_module(new EntityMgr);

	AppRef.run();

	return 0;
}
