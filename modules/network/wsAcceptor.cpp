#include "wsAcceptor.h"
#include <core/application.h>



gsf::network::WebSocketAcceptorModule::WebSocketAcceptorModule(const std::string &name)
	: Module(name)
{

}

gsf::network::WebSocketAcceptorModule::WebSocketAcceptorModule()
	: Module("WebSocketAcceptorModule")
{

}

gsf::network::WebSocketAcceptorModule::~WebSocketAcceptorModule()
{

}

void gsf::network::WebSocketAcceptorModule::before_init()
{
	using namespace std::placeholders;
	listen(eid::network::websocket_make_acceptor, std::bind(&WebSocketAcceptorModule::eMakeAcceptor, this, _1, _2));
}

void gsf::network::WebSocketAcceptorModule::init()
{
}

void gsf::network::WebSocketAcceptorModule::execute()
{
	
	
}

void gsf::network::WebSocketAcceptorModule::shut()
{
}

void gsf::network::WebSocketAcceptorModule::after_shut()
{

}

void gsf::network::WebSocketAcceptorModule::eMakeAcceptor(gsf::ModuleID target, gsf::ArgsPtr args)
{
	

}