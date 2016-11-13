#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <stdint.h>

#include <memory>
#include <unordered_map>

#include <event2/bufferevent.h>

namespace gsf
{
	class SessionCloseHandler;
	class SessionHandler;
	class Session;

	typedef std::shared_ptr<Session> SessionPtr;

	class SessionMgr
	{
		friend class Network;
	public:
		~SessionMgr();

		static SessionMgr & instance();

		int open(int session_id, std::shared_ptr<SessionHandler> session_handler, std::shared_ptr<SessionCloseHandler> close_handler);

		int close(int session_id);

		int send(int session_id, const uint8_t *data, uint32_t len);

	protected:
		SessionPtr make_session(::bufferevent *bev, int fd);

	private:
		static SessionMgr *instance_;

		typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
		SessionQueue session_queue_;
	};
}

#endif
