#include "session_mgr.h"

#include "session.h"

gsf::SessionMgr* gsf::SessionMgr::instance_ = nullptr;

gsf::SessionMgr& gsf::SessionMgr::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = new gsf::SessionMgr();
	}
	return *instance_;
}

int gsf::SessionMgr::open(int session_id, SessionCloseHandler *close_handler, SessionHandler *handler)
{
	return 0;
}
