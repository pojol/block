#ifndef _GSF_CONNECTOR_HEADER_
#define _GSF_CONNECTOR_HEADER_

#include <stdint.h>
#include <string>

namespace gsf
{
	class Session;
	class ConnectHandler;

	struct ConnectorConfig
	{
		int32_t port;
		std::string address;
	};

	class Connector
	{
	public:
		Connector(ConnectorConfig &config, ConnectHandler *handler);
		~Connector();

	private:
		ConnectorConfig config_;
		ConnectHandler *handler_;

		Session *session_;

		int32_t id_;
	};
}


#endif
