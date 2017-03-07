#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <stdint.h>

#include <memory>
#include <unordered_map>

#include "../../common/single.h"

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
		public:
			~SessionMgr();
			SessionMgr();

			void set_need_close(int fd);

			void close();

			SessionPtr find(int fd);

			SessionPtr make_session(::bufferevent *bev, int fd);

			int cur_max_connet() const;

		private:
			
			typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
			SessionQueue session_queue_;

			uint32_t session_index_;
		};
	}
}

#endif
