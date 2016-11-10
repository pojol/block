#ifndef _SESSION_MANAGER_HEADER_
#define _SESSION_MANAGER_HEADER_

namespace gsf
{
	class SessionMgr
	{
	public:
		~SessionMgr();

		static SessionMgr & instance();

		//int open(int session_id, SessionCloseHandler *close_handler, SessionHandler *handler);

		int close(int session_id);

		//binary
		//void send(int session_id, const BlockPtr &block);

		//logic
		//void send(int session_id, const MessagePtr &msg);

	private:
		static SessionMgr *instance_;
	};
}

#endif
