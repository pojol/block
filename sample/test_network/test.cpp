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

#include "../../common/single.h"

class AppFace
	: public gsf::utils::Singleton<AppFace>
	, public gsf::IEvent
{
public:

	void init(gsf::Application *app)
	{
		app_ = app;

		char _path[512];
		GetModuleFileName(NULL, _path, 512);
		//取出文件路径
		for (int i = strlen(_path); i >= 0; i--)
		{
			if (_path[i] == '\\')
			{
				_path[i] = '\0';
				break;
			}
		}

		//test
		dispatch2<gsf::modules::LogModule>(eid::log::init, gsf::Args(std::string(_path)));
	}

	void log_info(gsf::Args args);
	void log_warning(gsf::Args args);
	void log_err(gsf::Args args);

	template <typename M, typename T>
	void send_msg(IEvent *event_ptr, uint32_t fd, uint32_t msg_id, T msg);

	template <typename M>
	uint32_t get_module_id();

	template <typename M>
	void dispatch2(uint32_t eid, gsf::Args args);

private:
	gsf::Application *app_;
};

template <typename M>
void AppFace::dispatch2(uint32_t eid, gsf::Args args)
{
	dispatch(app_->find_module_id<M>(), eid, args);
}

template <typename M>
uint32_t AppFace::get_module_id()
{
	return app_->find_module_id<M>();
}

template <typename M, typename T>
void AppFace::send_msg(IEvent *event_ptr, uint32_t fd, uint32_t msg_id, T msg)
{
	app_->sendmsg<M>(event_ptr, fd, msg_id, msg);
}

void AppFace::log_info(gsf::Args args)
{
	dispatch(app_->find_module_id<gsf::modules::LogModule>(), eid::log::info, args);
}

void AppFace::log_warning(gsf::Args args)
{
	dispatch(app_->find_module_id<gsf::modules::LogModule>(), eid::log::warning, args);
}

void AppFace::log_err(gsf::Args args)
{
	dispatch(app_->find_module_id<gsf::modules::LogModule>(), eid::log::error, args);
}

#define Face AppFace::get_ref()

class Client2LoginServer
	: public gsf::network::AcceptorModule
{
public:
	Client2LoginServer()
		: AcceptorModule("Client2LoginServer")
	{}

private:

};

class EntityMgr
	: public gsf::Module
	, public gsf::IEvent
{
public:

	EntityMgr()
		: Module("EntityMgr")
	{}

	void init()
	{
		Face.log_info(gsf::Args(uint32_t(111), std::string(" info, log!")));
		Face.log_warning(gsf::Args(uint32_t(111), std::string(" warning, log!")));
		Face.log_err(gsf::Args(uint32_t(111), std::string(" err, log!")));

		uint32_t _em_id = Face.get_module_id<gsf::EventModule>();

		auto arr = {
			std::make_pair(uint32_t(1001), std::bind(&EntityMgr::test_remote, this, std::placeholders::_1, std::placeholders::_2)),
		};

		for (auto nod : arr)
		{
			//! 向协议绑定器申请，module 和 协议的绑定.
			dispatch(_em_id, eid::network::bind_remote_callback, gsf::Args(get_module_id(), nod.first, nod.second));
		}
	}

	void test_remote(uint32_t fd, gsf::BlockPtr blockptr)
	{
		test_network::Info _info;
		_info.ParseFromArray(blockptr->buf_, blockptr->size_);
		Face.log_info(gsf::Args(_info.id(), std::string(" "), _info.name()));

		_info.set_name("world");
		Face.send_msg<Client2LoginServer>(this, fd, 1002, _info);
	}

};

class Client2LoginProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	Client2LoginProxy()
		: Module("Client2LoginProxy")
	{}

	void init()
	{
		uint32_t _c2l_id = Face.get_module_id<Client2LoginServer>();

		listen(this, eid::network::new_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
		});

		listen(this, eid::network::dis_connect
			, [=](gsf::Args args, gsf::CallbackFunc callback){
			std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_module_id() << std::string("127.0.0.1") << uint32_t(8001);
		Face.dispatch2<Client2LoginServer>(eid::network::make_acceptor, args);
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

	gsf::Application *appptr = new gsf::Application();
	
	new AppFace;
	new gsf::EventModule();
	
	appptr->regist_module(gsf::EventModule::get_ptr());
	appptr->regist_module(new gsf::modules::LogModule());
	appptr->regist_module(new Client2LoginServer());
	appptr->regist_module(new Client2LoginProxy());
	appptr->regist_module(new EntityMgr());

	Face.init(appptr);
	appptr->run();

	return 0;
}
