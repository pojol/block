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

	class OBuffer;
	class IBuffer;

	typedef std::shared_ptr<Session> SessionPtr;

	class SessionMgr
	{
		friend class Network;
	public:
		~SessionMgr();

		static SessionMgr & instance();

		int open(int session_id, SessionHandler *session_handler, SessionCloseHandler *close_handler);

		int close(int session_id);

		int write(int session_id, const char *data, uint32_t len);

		SessionPtr find(int session_id);

	protected:
		SessionPtr make_session(::bufferevent *bev, int fd, OBuffer *out_buffer, IBuffer *in_buffer);

	private:
		static SessionMgr *instance_;

		typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
		SessionQueue session_queue_;
	};
}

#endif
