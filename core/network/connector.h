#ifndef _GSF_CONNECTOR_HEADER_
#define _GSF_CONNECTOR_HEADER_

#include <stdint.h>
#include <string>

namespace gsf
{
	class Session;

	struct ConnectorConfig
	{
		int32_t port;
		std::string address;
	};

	class ConnectHandler
	{
	public:
		virtual ~ConnectHandler(){}

		virtual void handler_new_connection(int connector_id, int session_id) = 0;
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
