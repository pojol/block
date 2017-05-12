#ifndef _GSF_MSG_BINDER_HEADER_

#include <map>
#include <event/event.h>

namespace gsf
{
	namespace network
	{
		struct RemoteInfo
		{
			RemoteInfo(uint32_t module_id, uint32_t msg_id, gsf::CallbackFunc func)
				: module_id_(module_id)
				, msg_id_(msg_id)
				, func_(func)
			{}

			uint32_t module_id_;
			uint32_t msg_id_;
			gsf::CallbackFunc func_;
		};

		typedef std::shared_ptr<RemoteInfo> RemoteInfoPtr;

		class MsgBinder
		{
			
		public:
			
			void regist(RemoteInfoPtr info_ptr);
			void unregist(uint32_t msg_id);
			
			gsf::CallbackFunc get_func(uint32_t msg_id);

		private:
			std::unordered_map<uint32_t, RemoteInfoPtr> remote_map_;
		};
	}
}

#endif // !_GSF_MSG_BINDER_HEADER_
