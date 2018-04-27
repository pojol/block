#include "wsConnector.h"

#include <core/application.h>



block::network::WebSocketConnectorModule::WebSocketConnectorModule(const std::string &name)
	: Module(name)
{

}

block::network::WebSocketConnectorModule::WebSocketConnectorModule()
	: Module("WebSocketAcceptorModule")
{

}

block::network::WebSocketConnectorModule::~WebSocketConnectorModule()
{

}

void block::network::WebSocketConnectorModule::before_init()
{
	using namespace std::placeholders;
	listen(eid::network::websocket_make_connector, std::bind(&WebSocketConnectorModule::eMakeConnector, this, _1, _2));
}

void block::network::WebSocketConnectorModule::init()
{
}

void block::network::WebSocketConnectorModule::execute()
{


}

void block::network::WebSocketConnectorModule::shut()
{

}

void block::network::WebSocketConnectorModule::after_shut()
{

}

void block::network::WebSocketConnectorModule::eMakeConnector(block::ModuleID target, block::ArgsPtr args)
{

}