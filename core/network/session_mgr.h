#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <stdint.h>

#include <memory>
#include <unordered_map>

#include <event2/bufferevent.h>

namespace gsf
{
	namespace network
	{
		class SessionCloseHandler;
		class SessionHandler;
		class Session;

		class OBuffer;
		class IBuffer;

		typedef std::shared_ptr<Session> SessionPtr;

		class SessionMgr
		{
			friend class NetworkImpl;

		public:
			~SessionMgr();
			SessionMgr(uint32_t index);

			int close(int session_id);

			SessionPtr find(int session_id);

		protected:
			SessionPtr make_session(::bufferevent *bev, int fd);

		private:
			
			typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
			SessionQueue session_queue_;

			uint32_t session_index_;
		};
	}
}

#endif
