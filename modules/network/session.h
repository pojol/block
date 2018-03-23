#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "msgBlock.h"

#include <map>
#include <memory>
#include <string>
#include <core/args.h>

namespace gsf
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
			Session(int fd, int eid, SessionMgr *mgr, ::bufferevent *bev);
			~Session();

			static void readCB(::bufferevent *bev, void *ctx);
			static void eventCB(::bufferevent *bev, short what, void *ctx);

			int write(uint32_t msg_id, BlockPtr blockptr);
			void read(::bufferevent *bev);

			void disConnect(int32_t err);
			void newConnect();

			gsf::SessionID getID() const { return fd_; }
			gsf::ModuleID getModuleID() const { return targetM_; }

		protected:

		private:
			gsf::SessionID fd_ = gsf::SessionNil;
			gsf::ModuleID targetM_ = gsf::ModuleNil;

			std::function<void(int)> disconnCallback_;

			::evbuffer *inBufPtr_ = nullptr;
			::evbuffer *outBufPtr_ = nullptr;

			::bufferevent *bufEvtPtr_ = nullptr;

			SessionMgr * basePtr_ = nullptr;
		};
	}
	
}


#endif
