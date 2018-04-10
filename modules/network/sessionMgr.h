#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <memory>
#include <vector>
#include <unordered_map>

#include <queue>

#include <core/single.h>
#include <core/module.h>
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

			void addClose(int fd);
			void addConnect(int fd);

			void addMessage(gsf::ArgsPtr args);

			void exec(MailBoxPtr mailbox);

			SessionPtr find(int fd);
			SessionPtr findByModule(uint32_t module_id);

			SessionPtr makeSession(int fd, int module_id, ::bufferevent *bev);

			int curMaxConnect() const;

		private:

			typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
			SessionQueue sessionQueue_;
			SessionQueue sessionQueueByModule_;

			std::vector<int> disconnectVec_;
			std::vector<int> connectVec_;

			std::queue<gsf::ArgsPtr> messageQueue_;

			gsf::ModuleID target_ = gsf::ModuleNil;
			uint32_t sessionIndex_;
		};
	}
}

#endif
