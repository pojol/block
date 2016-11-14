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

		virtual void handle_connect_failed(int connector_id, int err_code, const std::string &ip, const int port) = 0;
	};

	class Connector
	{
	public:
		Connector(uint32_t id, const ConnectorConfig &config);
		~Connector();

		uint32_t get_id() const { return id_; }

		void handle_connect_failed(int err_code, const std::string &ip, const int port);

	private:
		ConnectorConfig config_;
		ConnectHandler *handler_;

		Session *session_;

		uint32_t id_;
	};
}


#endif
