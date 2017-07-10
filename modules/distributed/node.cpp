#include "node.h"

#include <modules\network\acceptor.h>
#include <modules\network\connector.h>

#include <core/dynamic_module_factory.h>

#include <algorithm>


namespace gsf
{
	namespace network
	{
		REGISTER_CLASS(AcceptorModule)
		REGISTER_CLASS(ConnectorModule)
	}
}

gsf::modules::NodeModule::NodeModule()
	: Module("NodeModule")
{

}

gsf::modules::NodeModule::~NodeModule()
{

}

void gsf::modules::NodeModule::before_init()
{
	dispatch(eid::base::app_id, eid::get_module, gsf::make_args("LogModule"), [&](const gsf::ArgsPtr &args) {
		log_m_ = args->pop_i32();
	});
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
	

}

