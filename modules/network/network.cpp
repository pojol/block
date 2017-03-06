#include "network.h"

#include "network_imp.h"


gsf::network::NetworkModule::NetworkModule()
{

}

gsf::network::NetworkModule::~NetworkModule()
{

}

void gsf::network::NetworkModule::before_init()
{
	gsf::network::NetworkImpl::instance().init();
}

void gsf::network::NetworkModule::init()
{
	listen(event_id::network::start_network, std::bind(&NetworkModule::start, this
		, std::placeholders::_1
		, std::placeholders::_2));

	listen(event_id::network::make_acceptor, std::bind(&NetworkModule::make_acceptor, this
		, std::placeholders::_1
		, std::placeholders::_2));
}

void gsf::network::NetworkModule::execute()
{
	gsf::network::NetworkImpl::instance().execute();
}

void gsf::network::NetworkModule::shut()
{

}

void gsf::network::NetworkModule::after_shut()
{

}

void gsf::network::NetworkModule::start(gsf::Args args, gsf::EventHandlerPtr callback)
{
	gsf::network::NetworkImpl::instance().start();
}

void gsf::network::NetworkModule::make_acceptor(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _door = args.pop_uint32(0);
	std::string _ip = args.pop_string(1);
	uint32_t _port = args.pop_uint32(2);

	gsf::network::NetworkImpl::instance().make_acceptor(_ip, _port, callback);
}


