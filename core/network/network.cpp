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
	return NetworkImpl::instance().start();
}

int gsf::network::Network::listen(const AcceptorConfig &config, std::function<void(int)> func)
{
	return NetworkImpl::instance().make_acceptor(config, func);
}

void gsf::network::Network::write(uint32_t session_id, const char *data, uint32_t len)
{
	NetworkImpl::instance().write(session_id, data, len);
}

void gsf::network::Network::uninit()
{

}

void gsf::network::Network::regist_binder(Binder *binder)
{
	NetworkImpl::instance().regist_binder(binder);
}

