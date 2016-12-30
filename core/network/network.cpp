#include "network.h"

#include "network_imp.h"

gsf::network::Network* gsf::network::Network::instance_ = NULL;

gsf::network::Network::Network()
{

}

gsf::network::Network::~Network()
{

}

gsf::network::Network& gsf::network::Network::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::network::Network();
	}
	return *instance_;
}

int gsf::network::Network::init(const NetworkConfig &config)
{
	return NetworkImpl::instance().init(config);
}

int gsf::network::Network::start(UpdateFunc func)
{
	return NetworkImpl::instance().start(func);
}

int gsf::network::Network::listen(const std::string &ip, uint32_t port, NewConnectFunc newConnectFunc, DisConnectFunc disConnectFunc)
{
	return NetworkImpl::instance().make_acceptor(ip, port, newConnectFunc, disConnectFunc);
}

int gsf::network::Network::connect(const std::string &ip, uint32_t port, NewConnectFunc new_connect, ConnectFailedFunc connect_failed)
{
	return NetworkImpl::instance().make_connector(ip, port, new_connect, connect_failed);
}

void gsf::network::Network::write(uint32_t session_id, MessagePtr msg)
{
	NetworkImpl::instance().write(session_id, msg);
}

void gsf::network::Network::uninit()
{
	NetworkImpl::instance().uninit();
}

void gsf::network::Network::regist_binder(Binder *binder)
{
	NetworkImpl::instance().regist_binder(binder);
}

