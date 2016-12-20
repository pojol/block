#include "connector.h"

gsf::network::Connector::Connector(uint32_t id, const ConnectorConfig &config)
	:config_(config)
	,id_(id)
{

}

gsf::network::Connector::~Connector()
{

}

void gsf::network::Connector::err_cb(::bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)
	{
		/* connection has been closed, do any clean up here */
		printf("connection closed\n");
	}
	else if (what & BEV_EVENT_ERROR)
	{
		/* check errno to see what error occurred */
		printf("some other error\n");
	}
	else if (what & BEV_EVENT_TIMEOUT)
	{
		/* must be a timeout event handle, handle it */
		printf("Timed out\n");
	}
	bufferevent_free(bev);
}

void gsf::network::Connector::handle_connect_failed(int err_code, const std::string &ip, const int port)
{
	handler_->handle_connect_failed(id_, err_code, ip, port);
}