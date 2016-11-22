#include "connector.h"

gsf::network::Connector::Connector(uint32_t id, const ConnectorConfig &config)
	:config_(config)
	,id_(id)
{

}

gsf::network::Connector::~Connector()
{

}

void gsf::network::Connector::handle_connect_failed(int err_code, const std::string &ip, const int port)
{
	handler_->handle_connect_failed(id_, err_code, ip, port);
}