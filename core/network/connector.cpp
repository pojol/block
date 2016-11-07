#include "connector.h"

gsf::Connector::Connector(ConnectorConfig &config, ConnectHandler *handler)
	:config_(config)
	, handler_(handler)
{

}

gsf::Connector::~Connector()
{

}
