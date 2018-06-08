#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "msgBlock.h"

#include <map>
#include <memory>
#include <string>
#include <core/args.h>

namespace block
{
	namespace network
	{
		class OBuffer;
		class IBuffer;
		class MsgBinder;
		class SessionMgr;

		class Session
		{
		public:
			Session(int fd, block::ModuleID moduleID, SessionMgr *mgr, ::bufferevent *bev);
			~Session();

			static void readCB(::bufferevent *bev, void *ctx);
			static void eventCB(::bufferevent *bev, short what, void *ctx);

			int write(uint32_t msg_id, BlockPtr blockptr);
			void read(::bufferevent *bev);

			void disConnect(int32_t err);
			void newConnect();

			block::SessionID getID() const { return fd_; }
			block::ModuleID getModuleID() const { return targetM_; }

		protected:

		private:
			block::SessionID fd_ = block::SessionNil;
			block::ModuleID targetM_ = block::ModuleNil;

			std::function<void(int)> disconnCallback_;

			::evbuffer *inBufPtr_ = nullptr;
			::evbuffer *outBufPtr_ = nullptr;

			::bufferevent *bufEvtPtr_ = nullptr;

			SessionMgr * basePtr_ = nullptr;
		};
	}
	
}


#endif
