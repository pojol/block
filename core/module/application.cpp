#include "application.h"

#include <chrono>
#include <ctime>
#include <algorithm>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <module/dynamic_module_factory.h>

gsf::Application::Application()
	: Module("Application")
	, shutdown_(false)
	, delay_(20)
	, module_idx_(2)
	, state_(AppState::BEFORE_INIT)
	, cur_frame_(0)
{
	module_id_ = eid::app_id;

	std::list<std::function<void()>> before_init_list;
	before_init_list.push_back([&]() {
		auto _itr = module_list_.begin();
		while (_itr != module_list_.end())
		{
			(*_itr)->before_init();
			++_itr;
		}
	});
	call_list_[AppState::BEFORE_INIT] = before_init_list;

	std::list<std::function<void()>> init_list;
	init_list.push_back([&]() {
		auto _itr = module_list_.begin();
		while (_itr != module_list_.end())
		{
			(*_itr)->init();
			++_itr;
		}
	});
	call_list_[AppState::INIT] = init_list;

	std::list<std::function<void()>> execute_list;
	execute_list.push_back([&]() {
		auto _itr = module_list_.begin();
		while (_itr != module_list_.end())
		{
			(*_itr)->execute();
			++_itr;
		}
	});
	call_list_[AppState::EXECUTE] = execute_list;
}

void gsf::Application::init()
{
	new gsf::EventModule();

	listen(this, eid::get_module, [=](gsf::Args args, CallbackFunc callback) {
		
		std::string _name = args.pop_string(0);
	
		auto itr = module_name_map_.find(_name);
		if (itr != module_name_map_.end()) {
			callback(gsf::Args(itr->second));
		}

	});

	listen(this, eid::new_dynamic_module, [=](gsf::Args args, CallbackFunc callback) {

		std::string _name = args.pop_string(0);

		gsf::Module *_module_ptr = static_cast<gsf::Module*>(DynamicModuleFactory::create(_name));
		_module_ptr->set_id(make_module_id());

		push_frame(cur_frame_+1, std::make_tuple(0, std::bind(&Module::before_init, _module_ptr), nullptr, nullptr, _module_ptr));
		push_frame(cur_frame_+2, std::make_tuple(1, nullptr, std::bind(&Module::init, _module_ptr), nullptr, _module_ptr));
		push_frame(cur_frame_+3, std::make_tuple(2, nullptr, nullptr
			, std::bind(&Application::regist_module<Module>, this, std::placeholders::_1, std::placeholders::_2), _module_ptr));

		callback(gsf::Args(_module_ptr->get_module_id()));
	});

	listen(this, eid::delete_dynamic_module, [=](gsf::Args args, CallbackFunc callback) {

		uint32_t _module_id = args.pop_uint32(0);
		unregist_dynamic_module(_module_id);

	});
}

void gsf::Application::push_frame(uint64_t index, Frame frame)
{
	halfway_frame_.insert(std::make_pair(index, frame));
}

void gsf::Application::pop_frame()
{
	if (halfway_frame_.empty()) { return; }

	auto itr = halfway_frame_.find(cur_frame_);
	for (int i = 0; i < halfway_frame_.count(cur_frame_); ++i, ++itr)
	{
		int idx = std::get<0>(itr->second);
		if (idx == 0) {
			auto func = std::get<1>(itr->second);
			func();
		}
		else if (idx == 1) {
			auto func = std::get<2>(itr->second);
			func();
		}
		else if (idx == 2) {
			auto func = std::get<3>(itr->second);
			auto point = std::get<4>(itr->second);
			func(point, true);
		}
	}

	halfway_frame_.erase(cur_frame_);
}

void gsf::Application::run()
{
	//! run
	while (!shutdown_)
	{
		using namespace std::chrono;

		auto _before = time_point_cast<milliseconds>(system_clock::now());

		auto _callback = [&](std::list<std::function<void()>> calls) {
			for (auto call : calls)
			{
				call();
			}

			pop_frame();

			static_cast<Module*>(gsf::EventModule::get_ptr())->execute();
		};

		std::list<std::function<void()>> list_;
		if (state_ == AppState::BEFORE_INIT) {
			_callback(call_list_[AppState::BEFORE_INIT]);
			state_ = AppState::INIT;
		}
		else if (state_ == AppState::INIT) {
			_callback(call_list_[AppState::INIT]);
			state_ = AppState::EXECUTE;
		}
		else if (state_ == AppState::EXECUTE) {
			_callback(call_list_[AppState::EXECUTE]);
		}
		else if (state_ = AppState::SHUT) {
			_callback(call_list_[AppState::SHUT]);
			state_ = AppState::AFTER_SHUT;
		}
		else if (state_ = AppState::AFTER_SHUT) {
			_callback(call_list_[AppState::AFTER_SHUT]);
			//exit();
		}

		++cur_frame_;
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

}

uint32_t gsf::Application::make_module_id()
{
	if (module_idx_ == UINT32_MAX) {
		module_idx_ = 2;
	}

	return module_idx_++;
}

void gsf::Application::unregist_dynamic_module(uint32_t module_id)
{
	auto itr = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it){
		return (it->get_module_id() == module_id);
	});	

	if (itr != module_list_.end()){

		(*itr)->shut();
		(*itr)->after_shut();

		//! 

		module_list_.erase(itr);
	}
}