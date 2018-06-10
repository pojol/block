#include "module.h"
#include "application.h"

void block::Module::before_init()
{

}

void block::Module::init()
{

}

void block::Module::execute()
{

}

void block::Module::shut()
{

}

void block::Module::after_shut()
{

}

void block::Module::listen(block::EventID event, ListenFunc func)
{
	mailboxPtr_->listen(event, func);
}

void block::Module::dispatch(block::ModuleID target, block::EventID event, block::ArgsPtr args)
{
	mailboxPtr_->dispatch(target, event, std::move(args));
}

void block::Module::rpc(block::EventID event, ArgsPtr args, RpcCallback callback)
{
	mailboxPtr_->rpc(event, std::move(args), callback);
}

void block::Module::boardcast(block::EventID event, ArgsPtr args)
{
	mailboxPtr_->boardcast(event, std::move(args));
}

block::Module::Module(const std::string &name)
	: name_(name)
#ifdef WATCH_PERF
	, tick_consume_(0)
#endif // WATCH_PERF

{
	mailboxPtr_ = std::make_shared<MailBox>(this);
}

block::Module::~Module()
{

}

block::MailBox::MailBox(Module *ptr)
	: basePtr_(ptr)
{

}

void block::MailBox::listen(block::EventID event, ListenFunc func)
{
	auto _itr = listenMap_.find(event);
	if (_itr != listenMap_.end()) {
		WARN_FMTLOG("[BLOCK] mailbox repeat regist event:{}", event);
	}
	else {
		APP.reactorRegist(basePtr_->getModuleID(), event);

		listenMap_.insert(std::make_pair(event, func));
	}
}

void block::MailBox::dispatch(block::ModuleID target, block::EventID event, block::ArgsPtr args)
{
	APP.reactorDispatch(basePtr_->getModuleID(), target, event, std::move(args));
}

void block::MailBox::rpc(block::EventID event, ArgsPtr args, RpcCallback callback /*= nullptr*/)
{

}

void block::MailBox::boardcast(block::EventID event, ArgsPtr args)
{
	APP.reactorBoardcast(basePtr_->getModuleID(), event, std::move(args));
}

void block::MailBox::pull()
{
	while (!taskQueue_.empty())
	{
		TaskInfo * info = taskQueue_.front();

		auto _itr = listenMap_.find(info->event_);
		if (_itr != listenMap_.end()) {
			_itr->second(info->target_, std::move(info->args_));
		}
		else {
			WARN_FMTLOG("mailbox : pull can't find module : {}", basePtr_->getModuleID());
		}

		taskQueue_.pop();
	}
}

void block::MailBox::push(TaskInfo *info)
{
	taskQueue_.push(info);
}
