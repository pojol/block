#include "node.h"


gsf::distributed::NodeModule::NodeModule()
	: Module("NodeModule")
{

}

gsf::distributed::NodeModule::~NodeModule()
{

}

void gsf::distributed::NodeModule::init()
{
	using namespace std::placeholders;

	listen(this, eid::distributed::create_node, std::bind(&NodeModule::event_create_node, this, _1, _2));
}

void gsf::distributed::NodeModule::execute()
{

}

void gsf::distributed::NodeModule::shut()
{

}

void gsf::distributed::NodeModule::event_create_node(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _ip;
	auto _port;

	auto _vec;
	for (auto &it : _vec)
	{
		auto _id;
		auto _ip;
		auto _port;
	}

}

