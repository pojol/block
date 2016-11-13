#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <stdint.h>

namespace gsf
{
	class SessionCloseHandler;
	class SessionHandler;

	class SessionMgr
	{
	public:
		~SessionMgr();

		static SessionMgr & instance();

		int open(int session_id, SessionCloseHandler *close_handler, SessionHandler *handler);

		int close(int session_id);

		void send(int session_id, const uint8_t *data);

	private:
		static SessionMgr *instance_;
	};
}

#endif
