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

#include "module.h"
#include "dynamic_module_factory.h"

block::Application::Application()
	: shutdown_(false)
	, module_idx_(2)
	, state_(AppState::BEFORE_INIT)
	, cur_frame_(0)
{

}

std::string block::Application::getName() const
{
	return cfg_.name;
}

void block::Application::initCfg(const block::AppConfig &cfg)
{
	cfg_ = cfg;
	
	logger_ = std::make_shared<block::utils::Logger>();
	logger_->init(cfg_.name);
	timer_ = std::make_shared<block::utils::Timer>();

	//! 
	new block::ArgsPool;
	block::ArgsPool::get_ref().make(cfg.pool_args_count);
}

block::ModuleID block::Application::createDynamicModule(const std::string &moduleType)
{
	block::Module *_module_ptr = static_cast<block::Module*>(DynamicModuleFactory::create(moduleType));
	_module_ptr->setID(makeModuleID());

	registModule(_module_ptr, true);

	pushFrame(cur_frame_ + 1, std::make_tuple(0, std::bind(&Module::before_init, _module_ptr), nullptr, nullptr, _module_ptr));
	pushFrame(cur_frame_ + 2, std::make_tuple(1, nullptr, std::bind(&Module::init, _module_ptr), nullptr, _module_ptr));
	// 将regist提前，因为脚本层的module在动态创建的时候需要存在于module_list之中，否则会导致事件的注册失败。
	//pushFrame(cur_frame_ + 3, std::make_tuple(2, nullptr, nullptr
	//	, std::bind(&Application::registModule<Module>, this, std::placeholders::_1, std::placeholders::_2), _module_ptr));
	pushFrame(cur_frame_ + 3, std::make_tuple(2, nullptr, nullptr, nullptr, _module_ptr));

	return _module_ptr->getModuleID();
}

void block::Application::deleteModule(block::ModuleID moduleID)
{
	unregistModule(moduleID);
}

void block::Application::unregistModule(block::ModuleID module_id)
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
		WARN_FMTLOG("[BLOCK] unregistModule can't find module {}", module_id);
	}
}

block::ModuleID block::Application::getModule(const std::string &modulName) const
{
	auto itr = module_name_map_.find(modulName);
	if (itr != module_name_map_.end()) {
		return itr->second;
	}
	else {
		return block::ModuleNil;
	}
}

uint32_t block::Application::getMachine() const
{
	return cfg_.machine_;
}

int64_t block::Application::getUUID()
{
	return uuid();
}

void block::Application::pushFrame(uint64_t index, Frame frame)
{
	halfway_frame_.insert(std::make_pair(index, frame));
}

void block::Application::popFrame()
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
				//if (ptr->getAvailable()) {
					ptr->mailboxPtr_->pull();
				//}
				ptr->init();
			}
			else if (idx == 2) {
				auto point = std::get<4>(itr->second);
				point->setAvailable(true);
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
					if (_mitr != module_name_map_.end()) {
						module_name_map_.erase(_mitr);
					}
				}
				else {
					WARN_FMTLOG("[BLOCK] popFrame can't find module {}", _module_id);
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

void block::Application::reactorRegist(block::ModuleID moduleID, block::EventID event)
{
	auto _find = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it) {
		return (it->getModuleID() == moduleID);
	});

	if (_find != module_list_.end()) {

		mailboxMap_.insert(std::make_pair((moduleID * 10000) + event, (*_find)->mailboxPtr_));

	}
	else {
		WARN_FMTLOG("[BLOCK]application regist event:{} can't find module id:{}", event, moduleID);
	}
}

void block::Application::reactorDispatch(block::ModuleID self, block::ModuleID target, block::EventID event, block::ArgsPtr args)
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
		WARN_FMTLOG("[BLOCK] application dispatch, can't find event module:{}, event:{}", target, event);
		return;
	}
}

void block::Application::run()
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
					it->setAvailable(true);
				}
				else if (state == AppState::INIT) {
					if (it->getAvailable()) {
						it->mailboxPtr_->pull();
					}

					it->init();
				}
				else if (state == AppState::EXECUTE) {

					if (it->getAvailable()) {
						it->mailboxPtr_->pull();
						it->execute();
					}
#ifdef WATCH_PERF
					t1 = (time_point_cast<microseconds>(system_clock::now()) - _ttime).count();
					it->add_tick_consume(t1);
					_ttime = time_point_cast<microseconds>(system_clock::now());
#endif // WATCH_PERF
				}
				else if (state == AppState::SHUT) {

					if (it->getAvailable()) {
						it->mailboxPtr_->pull();
					}

					it->shut();

					it->setAvailable(false);
				}
				else if (state == AppState::AFTER_SHUT) {
					it->after_shut();
				}
				else {
					// waht?
				}
			}

			//! 动态创建的module，要先执行前面的状态帧。 才能同步到application状态中!
			popFrame();
			/*
			while (!unregist_list_.empty()) {
				auto itr = unregist_list_.front();
				unregist_dynamic_module(itr);
				unregist_list_.pop_front();
			}
			*/
			//static_cast<Module*>(block::EventModule::get_ptr())->execute();
#ifdef WATCH_PERF
			//t1 = (time_point_cast<microseconds>(system_clock::now()) - _ttime).count();
			//static_cast<Module*>(block::EventModule::get_ptr())->add_tick_consume(t1);
			//_ttime = time_point_cast<microseconds>(system_clock::now());
#endif // WATCH_PERF
		};

		timer_->exec();

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
		block::ArgsPool::get_ref().reenter();

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
			auto of = _use_ms - cfg_.tick_count;
			WARN_FMTLOG("single frame processing time overflow : {}ms", of);
		}
	}

}

void block::Application::exit()
{

}

int32_t block::Application::makeModuleID()
{
	if (module_idx_ == INT32_MAX) {
		module_idx_ = 2;
	}

	return module_idx_++;
}

uint64_t block::Application::getSystemTick()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t block::Application::uuid()
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


void block::Application::unregist_dynamic_module(uint32_t module_id)
{
	auto itr = std::find_if(module_list_.begin(), module_list_.end(), [&](std::list<Module *>::value_type it){
		return (it->getModuleID() == module_id);
	});	

	if (itr != module_list_.end()){

		(*itr)->shut();
		(*itr)->after_shut();

		//! 

		module_list_.erase(itr);
	}
}

void block::Application::tick()
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

			_pref += static_cast<Module*>(block::EventModule::get_ptr())->get_tick_info(tick_len_, cfg_.tick_count) + "\n";
			std::cout << _pref << block::ArgsPool::get_ref().get_perf();
		}

		last_tick_ = _t;
	}
	*/
}