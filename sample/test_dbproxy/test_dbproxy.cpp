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

	gsf::Application app;
	gsf::AppConfig cfg;
	app.init_cfg(cfg);

	app.regist_module(gsf::EventModule::get_ptr());
	app.regist_module(new gsf::modules::LogModule());
	app.regist_module(new TestModule());

	app.run();

	return 0;
}
