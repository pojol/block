#ifndef _CONNECT_HANDLER_HEADER_
#define _CONNECT_HANDLER_HEADER_

namespace gsf
{
	class ConnectHandler
	{
	public:
		virtual ~ConnectHandler(){}

		virtual void handler_new_connection(int connector_id, int session_id) = 0;
	};
}

#endif