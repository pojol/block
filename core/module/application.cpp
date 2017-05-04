#include "application.h"

#include <ctime>
#include <algorithm>
#include <string>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <module/dynamic_module_factory.h>



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
	cur_tick_time_ = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
#endif
}

void gsf::Application::init_cfg(const gsf::AppConfig &cfg)
{
	cfg_ = cfg;

	new gsf::EventModule;

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

		push_frame(cur_frame_ + 1, std::make_tuple(0, std::bind(&Module::before_init, _module_ptr), nullptr, nullptr, _module_ptr));
		push_frame(cur_frame_ + 2, std::make_tuple(1, nullptr, std::bind(&Module::init, _module_ptr), nullptr, _module_ptr));
		push_frame(cur_frame_ + 3, std::make_tuple(2, nullptr, nullptr
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

		auto _before = time_point_cast<microseconds>(system_clock::now());
		auto _ttime = time_point_cast<microseconds>(system_clock::now());

#ifdef WATCH_PERF
		double t1 = 0;
		double ratio = 0;
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
					ratio = t1 / (TICK_COUNT * 1000);
					it->add_tick_consume(ratio);
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

			static_cast<Module*>(gsf::EventModule::get_ptr())->execute();
#ifdef WATCH_PERF
			t1 = (time_point_cast<microseconds>(system_clock::now()) - _ttime).count();
			ratio = t1 / (TICK_COUNT * 1000);
			static_cast<Module*>(gsf::EventModule::get_ptr())->add_tick_consume(ratio);
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

		auto _use = time_point_cast<microseconds>(system_clock::now()) - _before;
		uint32_t _use_ms = static_cast<uint32_t>(_use.count() / 1000);
		if (_use_ms < TICK_COUNT) {
#ifdef WIN32
			Sleep(TICK_COUNT - _use_ms);
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

void gsf::Application::tick()
{
	if (cfg_.is_watch_pref){
		int _t = (last_tick_ + 1) % tick_len_;

		if (_t == 0) {
			std::string _pref = "------------------- pref -------------------\n";
			for (auto it : module_list_)
			{
				_pref += it->get_tick_info(tick_len_);
			}

			_pref += static_cast<Module*>(gsf::EventModule::get_ptr())->get_tick_info(tick_len_) + "\n";
			auto _ft = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()) - cur_tick_time_;
			_pref += "frame comsume : " + std::to_string(static_cast<uint32_t>(_ft.count() / 1000 / (tick_len_ * TICK_COUNT))) + " ms\n";
			std::cout << _pref << std::endl;

			cur_tick_time_ = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
			
		}

		last_tick_ = _t;
	}

}