#include "module.h"
#include "application.h"

void gsf::Module::before_init()
{

}

void gsf::Module::init()
{

}

void gsf::Module::execute()
{
	mailboxPtr_->pop();
}

void gsf::Module::shut()
{

}

void gsf::Module::after_shut()
{

}

gsf::Module::Module(const std::string &name)
	: name_(name)
#ifdef WATCH_PERF
	, tick_consume_(0)
#endif // WATCH_PERF

{
	mailboxPtr_ = std::make_shared<MailBox>(this);
}

gsf::Module::~Module()
{

}

gsf::MailBox::MailBox(Module *ptr)
	: basePtr_(ptr)
{

}

void gsf::MailBox::listen(gsf::EventID event, ListenFunc func)
{
	auto _itr = listenMap_.find(event);
	if (_itr != listenMap_.end()) {
		APP.WARN_LOG("mailbox", "repeat regist event!", " {}", event);
	}
	else {
		APP.reactorRegist(basePtr_->getModuleID(), event);

		listenMap_.insert(std::make_pair(event, func));
	}
}

void gsf::MailBox::dispatch(gsf::ModuleID target, gsf::EventID event, gsf::ArgsPtr args)
{
	APP.reactorDispatch(target, event, std::move(args));
}

void gsf::MailBox::rpc(gsf::EventID event, ArgsPtr args, RpcCallback callback /*= nullptr*/)
{

}

void gsf::MailBox::pop()
{
	while (taskQueue_.empty())
	{

		TaskInfo * info = taskQueue_.front();

		auto _itr = listenMap_.find(info->event_);
		if (_itr != listenMap_.end()) {
			_itr->second(info->target_, std::move(info->args_));
		}
		else {
			APP.WARN_LOG("mailbox", "", " {}", basePtr_->getModuleID());
		}

		taskQueue_.pop();
	}
}

void gsf::MailBox::push(TaskInfo *info)
{
	taskQueue_.push(info);
}
