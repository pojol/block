#include "event.h"
#include "application.h"
#include <algorithm>


void gsf::EventModule::execute()
{
	/*
	while (!eventQueue_.empty())
	{
		EventInfo *_infoPtr = eventQueue_.front();

		auto _reg = typeMap_.find(_infoPtr->target_);
		if (_reg != typeMap_.end()){
			auto _regList = _reg->second;
			auto _findItr = std::find_if(_regList.begin(), _regList.end(), [&](MIList::value_type it) {
				return (it.eventID_ == _infoPtr->eventID_);
			});

			if (_findItr != _regList.end()){
#ifdef WATCH_PERF
				_findItr->calls_++;
#endif
				_findItr->eventFunc_(std::move(_infoPtr->ptr_), _infoPtr->callback_);
			}
			else {
				APP.WARN_LOG("EventCenter", "execute Did not find the ", "event {} from module {} !", _infoPtr->eventID_, _infoPtr->target_);
			}
		}
		else {
			APP.WARN_LOG("EventCenter", "execute Did not find the module", " {}", _infoPtr->target_);
		}

		delete _infoPtr;
		_infoPtr = nullptr;

		eventQueue_.pop();
	}
	*/
}

gsf::EventModule::EventModule()
	: Module("EventModule")
{

}


void gsf::EventModule::bindEvent(uint32_t module_id, uint32_t event, DispatchFunc func)
{
	auto regf = [&](MIList &itr) {

		ModuleIterfaceObj _obj;
		_obj.eventID_ = event;
		_obj.eventFunc_ = func;

		itr.push_back(_obj);
	};

	auto typeItr = typeMap_.find(module_id);
	if (typeItr != typeMap_.end()) {

		auto listItr = typeItr->second;

		auto fItr = std::find_if(listItr.begin(), listItr.end(), [&](MIList::value_type it) {
			return (it.eventID_ == event);
		});

		if (fItr != listItr.end()) {
			APP.WARN_LOG("EventCenter", "bind_event repeated event", " {}", event);
			return;
		}

		regf(typeItr->second);
	}
	else {
		MIList _list;
		regf(_list);

		typeMap_.insert(std::make_pair(module_id, _list));
	}
}

void gsf::EventModule::dispatch(uint32_t module_id, uint32_t event, ArgsPtr args, CallbackFunc callback /* = nullptr */)
{
	/* 这里将来用actor模式做优化，暂时用同步的方式处理先保证逻辑可用。
		EventInfo *_einfo = new EventInfo();
		_einfo->eventID_ = event;
		_einfo->target_ = module_id;

		if (callback) {
			_einfo->callback_ = callback;
		}

		_einfo->ptr_ = std::move(args);
		eventQueue_.push(_einfo);
	*/

	auto _reg = typeMap_.find(module_id);
	if (_reg != typeMap_.end()) {
		auto _regList = _reg->second;
		auto _findItr = std::find_if(_regList.begin(), _regList.end(), [&](MIList::value_type it) {
			return (it.eventID_ == event);
		});

		if (_findItr != _regList.end()) {
#ifdef WATCH_PERF
			_findItr->calls_++;
#endif
			_findItr->eventFunc_(std::move(args), callback);
		}
		else {
			APP.WARN_LOG("EventCenter", "execute Did not find the ", "event {} from module {} !", event, module_id);
		}
	}
	else {
		APP.WARN_LOG("EventCenter", "execute Did not find the module", " {}", module_id);
	}
}

void gsf::EventModule::boardcast(uint32_t event, ArgsPtr args)
{
	for (auto &it : typeMap_)
	{
		dispatch(it.first, event, std::move(args));
	}
}

void gsf::EventModule::bindRpc(RpcFunc rpc_callback)
{
	rpc_ = rpc_callback;
}

void gsf::EventModule::dispatchRpc(uint32_t event, int32_t moduleid, ArgsPtr args, RpcCallback callback /* = nullptr */)
{
	assert(rpc_);
	rpc_(event, moduleid, std::move(args), callback);
}

void gsf::EventModule::rmvEvent(ModuleID module_id)
{
	auto tItr = typeMap_.find(module_id);

	if (tItr != typeMap_.end()) {
		tItr->second.clear();
	}
}

void gsf::EventModule::rmvEvent(ModuleID module_id, EventID event_id)
{
	auto tItr = typeMap_.find(module_id);

	if (tItr != typeMap_.end()) {
	
		auto listItr = tItr->second;
		auto fItr = std::find_if(listItr.begin(), listItr.end(), [&](MIList::value_type it) {
			return (it.eventID_ == event_id);
		});

		if (fItr != listItr.end()) {
			listItr.erase(fItr);
		}
	}
}

gsf::IEvent::IEvent()
{
}

gsf::IEvent::~IEvent()
{

}

void gsf::IEvent::listenRpc(RpcFunc callbackRpc)
{
	EventModule::get_ref().bindRpc(callbackRpc);
}

void gsf::IEvent::listen(Module *self, gsf::EventID event, DispatchFunc func)
{
	EventModule::get_ref().bindEvent(self->getModuleID(), event, func);
}

void gsf::IEvent::listen(ModuleID self, gsf::EventID event, DispatchFunc func)
{
	EventModule::get_ref().bindEvent(self, event, func);
}

void gsf::IEvent::dispatch(gsf::ModuleID target, gsf::EventID event, ArgsPtr args, CallbackFunc callback /* = nullptr */)
{
	EventModule::get_ref().dispatch(target, event, std::move(args), callback);
}

void gsf::IEvent::boardcast(uint32_t event, gsf::ArgsPtr args)
{
	EventModule::get_ref().boardcast(event, std::move(args));
}


void gsf::IEvent::rpc(uint32_t event, int32_t moduleid, ArgsPtr args, RpcCallback callback /* = nullptr */)
{
	EventModule::get_ref().dispatchRpc(event, moduleid, std::move(args), callback);
}

void gsf::IEvent::wipeout(ModuleID self)
{
	EventModule::get_ref().rmvEvent(self);
}

void gsf::IEvent::wipeout(Module *self, EventID eventID)
{
	EventModule::get_ref().rmvEvent(self->getModuleID(), eventID);
}

void gsf::IEvent::wipeout(Module *self)
{
	EventModule::get_ref().rmvEvent(self->getModuleID());
}

void gsf::IEvent::wipeout(ModuleID self, EventID eventID)
{
	EventModule::get_ref().rmvEvent(self, eventID);
}

