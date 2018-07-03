#include "application.h"


block::Application::Application()
{
}

std::string block::Application::getName() const
{
	return impl_.getName();
}

void block::Application::initCfg(const block::AppConfig &cfg)
{
	impl_.initCfg(cfg);
}

block::ModuleID block::Application::createDynamicModule(const std::string &moduleType)
{
	return impl_.createDynamicModule(moduleType);
}

void block::Application::deleteModule(block::ModuleID moduleID)
{
	impl_.deleteModule(moduleID);
}

block::ModuleID block::Application::getModuleID(const std::string &moduleName) const
{
	return impl_.getModuleID(moduleName);
}

bool block::Application::hasModule(block::ModuleID moduleid)
{
	return impl_.hasModule(moduleid);
}

const block::AppConfig & block::Application::getAppCfg()
{
	return impl_.getAppCfg();
}

int64_t block::Application::getUUID()
{
	return impl_.uuid();
}

void block::Application::run()
{
	impl_.run();
}

void block::Application::exit()
{
	impl_.exit();
}

uint64_t block::Application::getSystemTick()
{
	return impl_.getSystemTick();
}

block::utils::Logger block::Application::getLogger()
{
	return impl_.getLogger();
}

block::utils::Timer & block::Application::getTimer()
{
	return impl_.getTimer();
}

void block::Application::reactorRegist(block::ModuleID moduleID, block::EventID event)
{
	impl_.reactorRegist(moduleID, event);
}

void block::Application::reactorDispatch(block::ModuleID self, block::ModuleID target, block::EventID event, block::ArgsPtr args)
{
	impl_.reactorDispatch(self, target, event, std::move(args));
}

void block::Application::reactorBoardcast(block::ModuleID self, block::EventID event, block::ArgsPtr args)
{
	impl_.reactorBoardcast(self, event, std::move(args));
}
