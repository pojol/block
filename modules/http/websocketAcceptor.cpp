#include "websocketAcceptor.hpp"

#include <core/application.h>



gsf::network::WebSocketAcceptorModule::WebSocketAcceptorModule(const std::string &name)
	: Module(name)
{

}

gsf::network::WebSocketAcceptorModule::~WebSocketAcceptorModule()
{

}

void gsf::network::WebSocketAcceptorModule::before_init()
{
	
}

void gsf::network::WebSocketAcceptorModule::init()
{

}

void gsf::network::WebSocketAcceptorModule::execute()
{
	mailboxPtr_->pull();

	if (isRuning_) {
		server_.poll_one();
	}
}

void gsf::network::WebSocketAcceptorModule::shut()
{

}

void gsf::network::WebSocketAcceptorModule::after_shut()
{

}


void gsf::network::WebSocketAcceptorModule::eMakeAcceptor(gsf::ModuleID target, gsf::ArgsPtr args)
{
	auto _port = args->pop_i32();

	using namespace cinatra;

	server_.listen("0.0.0.0", std::to_string(_port));

	server_.set_http_handler<GET, POST>("/ws", [&](const request &req, response &res) {
	
		auto _state = req.get_state();
		switch (_state) 
		{
			case data_proc_state::data_begin:
			{
				WSSessionPtr _session = std::make_shared<WSSession>();
				_session->connPtr_ = req.get_conn()->shared_from_this();
				_session->tick_ = APP.getSystemTick();
				_session->uuid_ = APP.getUUID();
				sessionMap_.insert(std::make_pair(_session->uuid_, _session));



				break;
			}
			case data_proc_state::data_continue:			
			{
				break;
			}
			case data_proc_state::data_close:
			{
				break;
			}
			case data_proc_state::data_error:
			{	
				break;
			}
		}
		
	});

}