#include "node.h"


gsf::modules::NodeModule::NodeModule()
	: Module("NodeModule")
{

}

gsf::modules::NodeModule::~NodeModule()
{

}

void gsf::modules::NodeModule::before_init()
{

}

void gsf::modules::NodeModule::init()
{
	using namespace std::placeholders;

	

	listen(this, eid::distributed::create_node, std::bind(&NodeModule::event_create_node, this, _1, _2));
}

void gsf::modules::NodeModule::execute()
{

}

void gsf::modules::NodeModule::shut()
{

}

void gsf::modules::NodeModule::event_create_node(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _isListen = args->pop_bool();
	auto _name = args->pop_string();
	auto _ip = args->pop_string();
	auto _port = args->pop_i32();

	if (_isListen) {

	}

	auto _num = args->pop_i32();
	for (int i = 0; i < _num; ++i)
	{
		auto _connect_namee = args->pop_string();
		auto _connect_ip = args->pop_string();
		auto _connect_port = args->pop_i32();
	}

}

