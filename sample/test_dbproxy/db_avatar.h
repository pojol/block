#ifndef _DB_AVATAR_HEADER_
#define _DB_AVATAR_HEADER_

#include <module/module.h>
#include <event/event.h>

class db_avatar
	: public gsf::Module
	, public gsf::IEvent
{
public:
	db_avatar()
		: Module("db_avatar")
	{}

	void init() override;
	 
	void execute() override;

	void shut() override;

private:

	void event_update(gsf::Args args, gsf::CallbackFunc callback);
	
	void event_remove(gsf::Args args, gsf::CallbackFunc callback);

	void event_load(gsf::Args args, gsf::CallbackFunc callback);
	

private:
	// redisContext *read
	// redisContext *write
};

#endif // !_DB_AVATAR_HEADER_
