#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <memory>
#include <vector>
#include <unordered_map>

#include <core/single.h>

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
		class MsgBinder;

		typedef std::shared_ptr<Session> SessionPtr;

		class SessionMgr
		{
		public:
			~SessionMgr();
			SessionMgr();

			void setNeedClose(int fd);

			void close();

			SessionPtr find(int fd);
			SessionPtr findByModule(uint32_t module_id);

			SessionPtr makeSession(int fd, int module_id, MsgBinder *binder, ::bufferevent *bev);

			int curMaxConnect() const;

		private:

			typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
			SessionQueue sessionQueue_;
			SessionQueue sessionQueueByModule_;

			std::vector<int> disconnectVec_;

			uint32_t sessionIndex_;
		};
	}
}

#endif
