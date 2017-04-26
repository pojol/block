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
#else
#include <unistd.h>
#endif // WIN32

#include <module/application.h>
#include <event/event.h>

#include <network/acceptor.h>
#include <network/connector.h>

#include <log/log.h>

#include <iostream>

#include "db_avatar.h"

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
#ifdef WIN32
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
#else
		int cnt = readlink("/proc/self/exe", _path, 512);
		if (cnt < 0 || cnt >= 512) {
			std::cout << "read path err" << std::endl;
			return;
		}
		for (int i = cnt; i >= 0; --i)
		{
			if (_path[i] == '/') {
				_path[i + 1] = '\0';
				break;
			}
		}
#endif // WIN32

		//test
		dispatch2<gsf::modules::LogModule>(eid::log::init, gsf::Args(std::string(_path)
			, std::string("test_dbproxy")));
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

class TestModule
	: public gsf::Module
	, public gsf::IEvent
{
public:

	TestModule()
		: Module("TestModule")
	{}

	google::protobuf::Message * find_message(const std::string &key1, const std::string &key2)
	{
		
	}

	google::protobuf::Message * find_message(const std::string &key1)
	{
		
	}

	void init()
	{
		gsf::Args args(std::string("grids"), std::string("pos"), uint32_t(11));

		std::string _table_name = args.pop_string(0);
		std::string _field_name = args.pop_string(1);

		auto descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(_table_name);
		if (!descriptor) {
			assert(descriptor != nullptr);
			return;
		}

		//! 如果redis中没有
		//auto proto = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		//auto msg = proto->New();

		auto reflection = _avatar.GetReflection();
		if (!reflection) {
			assert(reflection != nullptr);
			return;
		}

		for (int i = 0; i < descriptor->field_count(); ++i)
		{
			auto field = descriptor->field(i);
			if (!field) {
				assert(field != nullptr);
				continue;
			}

			if (field->name() == _field_name) {
				switch (field->type())
				{
				case google::protobuf::FieldDescriptor::Type::TYPE_STRING:
					reflection->SetString(&_avatar, field, args.pop_string(2));
					break;
				case google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE:
					for each (auto var in _avatar.grids())
					{
						if (var.pos() == 1) {
							
						}

						
					}
					break;
				}
			}
		}

		for each (auto var in _avatar.grids())
		{
			std::cout << var.pos() << std::endl;
		}
	}

	Avatar _avatar;
	
};

int main()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(1, 1);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0) {
		exit(1);
	}
#endif // WIN32

	gsf::Application *appptr = new gsf::Application();

	new AppFace;
	new gsf::EventModule();

	appptr->regist_module(gsf::EventModule::get_ptr());
	appptr->regist_module(new gsf::modules::LogModule());
	appptr->regist_module(new TestModule());

	Face.init(appptr);
	appptr->init();
	appptr->run();

	return 0;
}
