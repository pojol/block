#include "application.h"

#include <chrono>
#include <ctime>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

gsf::Application::Application()
	: shutdown_(false)
	, delay_(20)
{

}

void gsf::Application::regist_module(Module *m)
{
	module_list_.push_back(m);
}

void gsf::Application::run()
{
	//! before init
	auto _itr = module_list_.begin();
	while (_itr != module_list_.end())
	{
		(*_itr)->before_init();
		++_itr;
	}

	//! init
	_itr = module_list_.begin();
	while (_itr != module_list_.end())
	{
		(*_itr)->init();
		++_itr;
	}

	//! run
	while (!shutdown_)
	{
		using namespace std::chrono;

		auto _before = time_point_cast<milliseconds>(system_clock::now());

		auto _itr = module_list_.begin();
		while (_itr != module_list_.end())
		{
			(*_itr)->execute();
			++_itr;
		}

		tick();

		auto _use = time_point_cast<milliseconds>(system_clock::now()) - _before;
		
		if (_use.count() < 20) {
#ifdef WIN32
			delay_ = static_cast<uint32_t>(20 - _use.count());
			Sleep(delay_);
#endif // WIN32
		}
		else {

		}
	}

	//! 
	_itr = module_list_.begin();
	while (_itr != module_list_.end())
	{
		(*_itr)->shut();
		++_itr;
	}

	//!
	_itr = module_list_.begin();
	while (_itr != module_list_.end())
	{
		(*_itr)->after_shut();
		++_itr;
	}

}