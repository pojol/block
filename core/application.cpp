#include "application.h"

#include <ctime>
#include <algorithm>
#include <iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#include "dynamic_module_factory.h"


gsf::Application::Application()
	: Module("Application")
	, shutdown_(false)
	, module_idx_(2)
	, state_(AppState::BEFORE_INIT)
	, cur_frame_(0)
{
	module_id_ = eid::app_id;

#ifdef WATCH_PERF
	tick_len_ = 200;
	last_tick_ = -1;
#endif
}

void gsf::Application::init_cfg(const gsf::AppConfig &cfg)
{
	cfg_ = cfg;

	new gsf::EventModule;
	new gsf::ArgsPool;
	gsf::ArgsPool::get_ref().make(cfg.pool_args_count);


	listen(this, eid::get_module, [=](const gsf::ArgsPtr &args) {

		std::string _name = args->pop_string();

		auto itr = module_name_map_.find(_name);
		if (itr != module_name_map_.end()) {
			return gsf::make_args(itr->second);
		}
		else {
			return gsf::make_args(gsf::ModuleNil);
		}

	});

	listen(this, eid::get_app_name, [=](const gsf::ArgsPtr &args){
		return gsf::make_args(cfg_.name);
	});

	listen(this, eid::new_dynamic_module, [=](const gsf::ArgsPtr &args) {

		std::string _name = args->pop_string();

		gsf::Module *_module_ptr = static_cast<gsf::Module*>(DynamicModuleFactory::create(_name));
		_module_ptr->set_id(make_module_id());

		push_frame(cur_frame_ + 1, std::make_tuple(0, std::bind(&Module::before_init, _module_ptr), nullptr, nullptr, _module_ptr));
		push_frame(cur_frame_ + 2, std::make_tuple(1, nullptr, std::bind(&Module::init, _module_ptr), nullptr, _module_ptr));
		push_frame(cur_frame_ + 3, std::make_tuple(2, nullptr, nullptr
			, std::bind(&Application::regist_module<Module>, this, std::placeholders::_1, std::placeholders::_2), _module_ptr));

		return gsf::make_args(_module_ptr->get_module_id());
	});

	listen(this, eid::delete_module, [=](const gsf::ArgsPtr &args) {
		uint32_t _module_id = args->pop_moduleid();
		unregist_module(_module_id);

		return nullptr;
	});
}

void gsf::Application::unregist_module(gsf::ModuleID module_id)
{
	auto itr = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it) {
		return (it->get_module_id() == module_id);
	});

	if (itr != module_list_.end()) {
	
		auto module_ptr = *itr;

		exit_list_.insert(std::make_pair(cur_frame_, std::make_pair(0, module_ptr))); // 在循环外处理清理逻辑
		exit_list_.insert(std::make_pair(cur_frame_ + 1, std::make_pair(1, module_ptr)));
		exit_list_.insert(std::make_pair(cur_frame_ + 2, std::make_pair(2, module_ptr)));
		exit_list_.insert(std::make_pair(cur_frame_ + 3, std::make_pair(3, module_ptr)));

	}
	else {
		std::cout << "unregist_module not find module " << module_id << std::endl;
	}
}

void gsf::Application::push_frame(uint64_t index, Frame frame)
{
	halfway_frame_.insert(std::make_pair(index, frame));
}

void gsf::Application::pop_frame()
{
	if (!halfway_frame_.empty()) {
		auto itr = halfway_frame_.find(cur_frame_);
		for (size_t i = 0; i < halfway_frame_.count(cur_frame_); ++i, ++itr)
		{
			int idx = std::get<0>(itr->second);
			if (idx == 0) {
				auto func = std::get<1>(itr->second);
				auto ptr = std::get<4>(itr->second);
				//func();
				ptr->before_init();
			}
			else if (idx == 1) {
				auto func = std::get<2>(itr->second);
				auto ptr = std::get<4>(itr->second);
				//func();
				ptr->init();
			}
			else if (idx == 2) {
				auto func = std::get<3>(itr->second);
				auto point = std::get<4>(itr->second);
				func(point, true);
			}
		}

		halfway_frame_.erase(cur_frame_);
	}

	if (!exit_list_.empty()) {

		auto itr = exit_list_.find(cur_frame_);
		for (size_t i = 0; i < exit_list_.count(cur_frame_); ++i, ++itr)
		{
			auto idx = itr->second.first;
			if (idx == 0) {

				auto _module_id = (itr->second.second)->get_module_id();
				auto _module_name = (itr->second.second)->get_module_name();
				auto litr = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it) {
					return (it->get_module_id() == _module_id);
				});

				if (litr != module_list_.end()) {
					module_list_.erase(litr);

					auto _mitr = module_name_map_.find(_module_name);
					assert(_mitr != module_name_map_.end());
					module_name_map_.erase(_mitr);
				}
				else {
					std::cout << "unregist_module pop frame not find module " << _module_id << std::endl;
				}
			}
			else if (idx == 1) {
				(itr->second.second)->shut();
			}
			else if (idx == 2) {
				(itr->second.second)->after_shut();
			}
			else {
				delete itr->second.second;
				itr->second.second = nullptr;
			}
		}

		exit_list_.erase(cur_frame_);
	}

}

void gsf::Application::run()
{
	//! run
	while (!shutdown_)
	{
		using namespace std::chrono;

		auto _before = time_point_cast<microseconds>(system_clock::now());
		auto _ttime = time_point_cast<microseconds>(system_clock::now());

#ifdef WATCH_PERF
		int64_t t1 = 0;
#endif // WATCH_PERF
		
		auto _callback = [&](AppState state) {
			for (auto it : module_list_)
			{
				if (state == AppState::BEFORE_INIT) {
					it->before_init();
				}
				else if (state == AppState::INIT) {
					it->init();
				}
				else if (state == AppState::EXECUTE) {
					it->execute();
#ifdef WATCH_PERF
					t1 = (time_point_cast<microseconds>(system_clock::now()) - _ttime).count();
					it->add_tick_consume(t1);
					_ttime = time_point_cast<microseconds>(system_clock::now());
#endif // WATCH_PERF
				}
				else if (state == AppState::SHUT) {
					it->shut();
				}
				else if (state == AppState::AFTER_SHUT) {
					it->after_shut();
				}
				else {
					// waht?
				}
			}

			pop_frame();
			//while (!unregist_list_.empty()) {
			//	auto itr = unregist_list_.front();
			//	unregist_dynamic_module(itr);
			//	unregist_list_.pop_front();
			//}

			static_cast<Module*>(gsf::EventModule::get_ptr())->execute();
#ifdef WATCH_PERF
			t1 = (time_point_cast<microseconds>(system_clock::now()) - _ttime).count();
			static_cast<Module*>(gsf::EventModule::get_ptr())->add_tick_consume(t1);
			_ttime = time_point_cast<microseconds>(system_clock::now());
#endif // WATCH_PERF
		};

		_callback(state_);
		if (state_ == AppState::BEFORE_INIT) {
			state_ = AppState::INIT;
		}
		else if (state_ == AppState::INIT) {
			state_ = AppState::EXECUTE;
		}
		else if (state_ == AppState::EXECUTE) {

		}
		else if (state_ = AppState::SHUT) {
			state_ = AppState::AFTER_SHUT;
		}
		else if (state_ = AppState::AFTER_SHUT) {
			//exit();
		}

		++cur_frame_;
		tick();
		gsf::ArgsPool::get_ref().reenter();

		auto _use = time_point_cast<microseconds>(system_clock::now()) - _before;
		uint32_t _use_ms = static_cast<uint32_t>(_use.count() / 1000);
		if (_use_ms < cfg_.tick_count) {
#ifdef WIN32
			Sleep(cfg_.tick_count - _use_ms);
#else
			usleep(1000 * (cfg_.tick_count - _use_ms));
#endif // WIN32
		}
		else {

		}
	}

}

int32_t gsf::Application::make_module_id()
{
	if (module_idx_ == INT32_MAX) {
		module_idx_ = 2;
	}

	return module_idx_++;
}

/*
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
*/

void gsf::Application::tick()
{
	if (cfg_.is_watch_pref){
		int _t = (last_tick_ + 1) % tick_len_;

		if (_t == 0) {
			std::string _pref = "------------------- pref -------------------\n";
			for (auto it : module_list_)
			{
				_pref += it->get_tick_info(tick_len_, cfg_.tick_count);
			}

			_pref += static_cast<Module*>(gsf::EventModule::get_ptr())->get_tick_info(tick_len_, cfg_.tick_count) + "\n";
			std::cout << _pref << gsf::ArgsPool::get_ref().get_perf();
		}

		last_tick_ = _t;
	}

}