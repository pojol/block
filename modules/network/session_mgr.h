#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

#include <stdint.h>

#include <memory>
#include <vector>
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
		class MsgBinder;

		typedef std::shared_ptr<Session> SessionPtr;

		class SessionMgr
		{
		public:
			~SessionMgr();
			SessionMgr();

			void set_need_close(int fd);

			void close();

			SessionPtr find(int fd);
			SessionPtr find_byModule(uint32_t module_id);

			SessionPtr make_session(int fd, int module_id, MsgBinder *binder);

			int cur_max_connet() const;

		private:

			typedef std::unordered_map<int32_t, SessionPtr> SessionQueue;
			SessionQueue session_queue_;
			SessionQueue session_queue_bymodule_;

			std::vector<int> disconnect_vec_;

			uint32_t session_index_;
		};
	}
}

#endif
