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
	: shutdown_(false)
	, module_idx_(2)
	, state_(AppState::BEFORE_INIT)
	, cur_frame_(0)
{
#ifdef WATCH_PERF
	tick_len_ = 200;
	last_tick_ = -1;
#endif
}

std::string gsf::Application::getAppName() const
{
	return cfg_.name;
}

void gsf::Application::initCfg(const gsf::AppConfig &cfg)
{
	cfg_ = cfg;
	
	//! 
	new gsf::ArgsPool;
	gsf::ArgsPool::get_ref().make(cfg.pool_args_count);
}

gsf::ModuleID gsf::Application::createDynamicModule(const std::string &moduleType)
{
	gsf::Module *_module_ptr = static_cast<gsf::Module*>(DynamicModuleFactory::create(moduleType));
	_module_ptr->setID(makeModuleID());

	pushFrame(cur_frame_ + 1, std::make_tuple(0, std::bind(&Module::before_init, _module_ptr), nullptr, nullptr, _module_ptr));
	pushFrame(cur_frame_ + 2, std::make_tuple(1, nullptr, std::bind(&Module::init, _module_ptr), nullptr, _module_ptr));
	pushFrame(cur_frame_ + 3, std::make_tuple(2, nullptr, nullptr
		, std::bind(&Application::registModule<Module>, this, std::placeholders::_1, std::placeholders::_2), _module_ptr));

	return _module_ptr->getModuleID();
}

void gsf::Application::deleteModule(gsf::ModuleID moduleID)
{
	unregistModule(moduleID);
}

void gsf::Application::unregistModule(gsf::ModuleID module_id)
{
	auto itr = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it) {
		return (it->getModuleID() == module_id);
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

gsf::ModuleID gsf::Application::getModule(const std::string &modulName) const
{
	auto itr = module_name_map_.find(modulName);
	if (itr != module_name_map_.end()) {
		return itr->second;
	}
	else {
		return gsf::ModuleNil;
	}
}

uint32_t gsf::Application::getMachine() const
{
	return cfg_.machine_;
}

int64_t gsf::Application::getUUID()
{
	return uuid();
}

void gsf::Application::pushFrame(uint64_t index, Frame frame)
{
	halfway_frame_.insert(std::make_pair(index, frame));
}

void gsf::Application::popFrame()
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

				auto _module_id = (itr->second.second)->getModuleID();
				auto _module_name = (itr->second.second)->getModuleName();
				auto litr = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it) {
					return (it->getModuleID() == _module_id);
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

void gsf::Application::reactorRegist(gsf::ModuleID moduleID, gsf::EventID event)
{
	auto _find = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it) {
		return (it->getModuleID() == moduleID);
	});

	if (_find != module_list_.end()) {

		mailboxMap_.insert(std::make_pair((moduleID * 10000) + event, (*_find)->mailboxPtr_));

	}
	else {
		WARN_LOG("app", "regist event, can't find module", " {}", moduleID);
	}
}

void gsf::Application::reactorDispatch(gsf::ModuleID self, gsf::ModuleID target, gsf::EventID event, gsf::ArgsPtr args)
{
	//! 找到目标mailbox
	auto _find = mailboxMap_.find((target * 10000) + event);
	if (_find != mailboxMap_.end()) {

		auto taskPtr = new TaskInfo();
		taskPtr->target_ = self;
		taskPtr->event_ = event;
		taskPtr->args_ = std::move(args);

		//! 将task加入到mailbox的FIFO队列
		_find->second->push(taskPtr);
	}
	else {
		WARN_LOG("app", "dispatch, can't find event", " module={} event={}", target, event);
		return;
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

			popFrame();
			//while (!unregist_list_.empty()) {
			//	auto itr = unregist_list_.front();
			//	unregist_dynamic_module(itr);
			//	unregist_list_.pop_front();
			//}

			//static_cast<Module*>(gsf::EventModule::get_ptr())->execute();
#ifdef WATCH_PERF
			//t1 = (time_point_cast<microseconds>(system_clock::now()) - _ttime).count();
			//static_cast<Module*>(gsf::EventModule::get_ptr())->add_tick_consume(t1);
			//_ttime = time_point_cast<microseconds>(system_clock::now());
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
		else if (state_ == AppState::SHUT) {
			state_ = AppState::AFTER_SHUT;
		}
		else if (state_ == AppState::AFTER_SHUT) {
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

void gsf::Application::exit()
{

}

int32_t gsf::Application::makeModuleID()
{
	if (module_idx_ == INT32_MAX) {
		module_idx_ = 2;
	}

	return module_idx_++;
}

uint64_t gsf::Application::getSystemTick()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t gsf::Application::uuid()
{
	int64_t _uuid = 0;
	uint64_t _tick = getSystemTick() - start_time_;
	
	_uuid |= _tick << 22;
	_uuid |= cfg_.machine_ & 0x3FF << 12;

	_uuid |= sequence_++ & 0xFFF;
	if (sequence_ == 0x1000) {
		sequence_ = 0;
	}

	return _uuid;
}


void gsf::Application::WARN_LOG(std::string module_name, std::string reason)
{
	std::string _str = "";
	_str.append("[module] ");
	_str.append(module_name);
	_str.append(" [reason] ");
	_str.append(reason);
	_str.append("\n");
	reactorDispatch(0, getModule("LogModule"), eid::log::print, gsf::makeArgs(gsf::LogWarning, _str));
}


void gsf::Application::ERR_LOG(std::string module_name, std::string reason)
{
	std::string _str = "";
	_str.append("[module] ");
	_str.append(module_name);
	_str.append(" [reason] ");
	_str.append(reason);
	_str.append("\n");
	reactorDispatch(0, getModule("LogModule"), eid::log::print, gsf::makeArgs(gsf::LogErr, _str));
}

void gsf::Application::INFO_LOG(std::string module_name, std::string reason)
{
	std::string _str = "";
	_str.append("[module] ");
	_str.append(module_name);
	_str.append(" [reason] ");
	_str.append(reason);
	_str.append("\n");
	reactorDispatch(0, getModule("LogModule"), eid::log::print, gsf::makeArgs(gsf::LogInfo, _str));
}

void gsf::Application::DEBUG_LOG(std::string module_name, std::string reason)
{
	std::string _str = "";
	_str.append("[module] ");
	_str.append(module_name);
	_str.append(" [reason] ");
	_str.append(reason);
	_str.append("\n");
	reactorDispatch(0, getModule("LogModule"), eid::log::print, gsf::makeArgs(gsf::LogDebug, _str));
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
	/*
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
	*/
}