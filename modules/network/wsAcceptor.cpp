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
	mailboxPtr_->listen(eid::network::websocket_make_acceptor, std::bind(&WebSocketAcceptorModule::eMakeAcceptor, this, _1, _2));
}

void gsf::network::WebSocketAcceptorModule::init()
{
	mailboxPtr_->pull();
}

void gsf::network::WebSocketAcceptorModule::execute()
{
	mailboxPtr_->pull();

	
}

void gsf::network::WebSocketAcceptorModule::shut()
{
	mailboxPtr_->pull();
}

void gsf::network::WebSocketAcceptorModule::after_shut()
{

}

void gsf::network::WebSocketAcceptorModule::eMakeAcceptor(gsf::ModuleID target, gsf::ArgsPtr args)
{
	

}