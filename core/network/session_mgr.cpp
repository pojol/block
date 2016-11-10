#include "session_mgr.h"


gsf::SessionMgr* gsf::SessionMgr::instance_ = nullptr;

gsf::SessionMgr& gsf::SessionMgr::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = new gsf::SessionMgr();
	}
	return *instance_;
}
