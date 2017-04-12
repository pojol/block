#include "db_avatar.h"

void db_avatar::init()
{
	//auto _eid = make_event_id(eid::db_proxy::update);

	listen(this, get_module_id()
		, std::bind(&db_avatar::event_update, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, get_module_id()
		, std::bind(&db_avatar::event_remove, this
			, std::placeholders::_1
			, std::placeholders::_2));

	listen(this, get_module_id()
		, std::bind(&db_avatar::event_load, this
			, std::placeholders::_1
			, std::placeholders::_2));
}

void db_avatar::execute()
{

}

void db_avatar::shut()
{

}


void db_avatar::event_update(gsf::Args args, gsf::CallbackFunc callback)
{

}

void db_avatar::event_remove(gsf::Args args, gsf::CallbackFunc callback)
{

}

void db_avatar::event_load(gsf::Args args, gsf::CallbackFunc callback)
{

}