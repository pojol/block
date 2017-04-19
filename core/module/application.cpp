#include "application.h"

#include <chrono>
#include <ctime>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <module/dynamic_module_factory.h>

gsf::Application::Application()
	: Module("Application")
	, shutdown_(false)
	, delay_(20)
	, module_idx_(2)
{
	module_id_ = eid::app_id;
}

void gsf::Application::init()
{
	listen(this, eid::get_module, [=](gsf::Args args, CallbackFunc callback) {
		
		std::string _name = args.pop_string(0);
	
		auto itr = module_name_map_.find(_name);
		if (itr != module_name_map_.end()) {
			callback(gsf::Args(itr->second));
		}

	});

	listen(this, eid::create_dynamic_module, [=](gsf::Args args, CallbackFunc callback) {

		std::string _name = args.pop_string(0);

		gsf::Module *_module_ptr = static_cast<gsf::Module*>(DynamicModuleFactory::create(_name));
		regist_module(_module_ptr, true);
		
		_module_ptr->before_init();
		_module_ptr->init();

		callback(gsf::Args(_module_ptr->get_module_id()));
	});
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

uint32_t gsf::Application::make_module_id()
{
	if (module_idx_ == UINT32_MAX) {
		module_idx_ = 2;
	}

	return module_idx_++;
}
