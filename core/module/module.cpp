#include "module.h"

void gsf::Module::before_init()
{

}

void gsf::Module::init()
{

}

void gsf::Module::execute()
{

}

void gsf::Module::shut()
{

}

void gsf::Module::after_shut()
{

}

gsf::Module::Module(const std::string &name)
	: name_(name)
#ifdef WATCH_PERF
	, tick_consume_(0)
#endif // WATCH_PERF

{

}

gsf::Module::~Module()
{

}
