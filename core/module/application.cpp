#include "application.h"

#include <chrono>
#include <ctime>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

gsf::core::Application::Application()
	: shutdown_(false)
{

}

void gsf::core::Application::regist_module(Module *m)
{
	module_list_.push_back(m);
}

void gsf::core::Application::run()
{
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

		auto _use = time_point_cast<milliseconds>(system_clock::now()) - _before;
		
		if (_use.count() < 20) {
#ifdef WIN32
			Sleep(20 - _use.count());
#endif // WIN32
		}
		else {

		}
	}

}