#include "wsAcceptor.h"
#include <core/application.h>



block::network::WebSocketAcceptorModule::WebSocketAcceptorModule(const std::string &name)
	: Module(name)
{

}

block::network::WebSocketAcceptorModule::WebSocketAcceptorModule()
	: Module("WebSocketAcceptorModule")
{

}

block::network::WebSocketAcceptorModule::~WebSocketAcceptorModule()
{

}

void block::network::WebSocketAcceptorModule::before_init()
{
	using namespace std::placeholders;
	listen(eid::network::websocket_make_acceptor, std::bind(&WebSocketAcceptorModule::eMakeAcceptor, this, _1, _2));
}

void block::network::WebSocketAcceptorModule::init()
{
}

void block::network::WebSocketAcceptorModule::execute()
{
	
	
}

void block::network::WebSocketAcceptorModule::shut()
{
}

void block::network::WebSocketAcceptorModule::after_shut()
{

}

void block::network::WebSocketAcceptorModule::eMakeAcceptor(block::ModuleID target, block::ArgsPtr args)
{
	

}