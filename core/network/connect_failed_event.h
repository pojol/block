#ifndef _GSF_CONNECT_FAILED_EVENT_HEADER_
#define _GSF_CONNECT_FAILED_EVENT_HEADER_

namespace gsf
{
	namespace network
	{
		class ConnectFailedEvent
		{
		public:
			virtual ~ConnectFailedEvent(){}

			virtual void handler_connect_failed() = 0;
		};
	}
}

#endif
