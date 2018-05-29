#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <memory>
#include <vector>
#include <unordered_map>

#include <queue>

#include <core/module.h>
#include <event2/bufferevent.h>

namespace block
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
			SessionMgr(Module *base);

			void addClose(int fd);
			void addConnect(int fd);

			void addMessage(block::ArgsPtr args);

			void exec();

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

			std::queue<block::ArgsPtr> messageQueue_;

			block::ModuleID target_ = block::ModuleNil;
			uint32_t sessionIndex_;

			// tmp 
			block::Module *basePtr_;
		};
	}
}

#endif
