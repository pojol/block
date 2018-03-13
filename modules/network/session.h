#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <core/event.h>
#include "msgBlock.h"

#include <map>
#include <memory>
#include <string>

namespace gsf
{
	namespace network
	{
		class OBuffer;
		class IBuffer;
		class MsgBinder;

		class Session
			: public gsf::IEvent
		{
		public:
			Session(int fd, int eid, MsgBinder *binder, std::function<void (int)> disconnect_callback, ::bufferevent *bev);
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

			MsgBinder *binder_ = nullptr;

			std::function<void(int)> disconnCallback_;

			::evbuffer *inBufPtr_ = nullptr;
			::evbuffer *outBufPtr_ = nullptr;

			::bufferevent *bufEvtPtr_ = nullptr;
		};
	}
	
}


#endif
