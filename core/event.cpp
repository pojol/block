#include "event.h"
#include "application.h"
#include <algorithm>


void gsf::EventModule::execute()
{
	while (!event_queue_.empty())
	{
		EventInfo *_infoPtr = event_queue_.front();

		auto _reg = type_map_.find(_infoPtr->target_);
		if (_reg != type_map_.end()){
			auto _regList = _reg->second;
			auto _findItr = std::find_if(_regList.begin(), _regList.end(), [&](MIList::value_type it) {
				return (it.event_id_ == _infoPtr->event_);
			});

			if (_findItr != _regList.end()){
#ifdef WATCH_PERF
				_findItr->calls_++;
#endif
				_findItr->event_func_(std::move(_infoPtr->ptr_), _infoPtr->callback_);
			}
			else {
				APP.WARN_LOG("EventCenter", "execute Did not find the ", "event {} from module {} !", _infoPtr->event_, _infoPtr->target_);
			}
		}
		else {
			APP.WARN_LOG("EventCenter", "execute Did not find the module", " {}", _infoPtr->target_);
		}

		delete _infoPtr;
		_infoPtr = nullptr;

		event_queue_.pop();
	}
}

gsf::EventModule::EventModule()
	: Module("EventModule")
{

}


void gsf::EventModule::bind_event(uint32_t module_id, uint32_t event, DispatchFunc func)
{
	auto regf = [&](MIList &itr) {

		ModuleIterfaceObj _obj;
		_obj.event_id_ = event;
		_obj.event_func_ = func;

		itr.push_back(_obj);
	};

	auto typeItr = type_map_.find(module_id);
	if (typeItr != type_map_.end()) {

		auto listItr = typeItr->second;

		auto fItr = std::find_if(listItr.begin(), listItr.end(), [&](MIList::value_type it) {
			return (it.event_id_ == event);
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

		type_map_.insert(std::make_pair(module_id, _list));
	}
}

void gsf::EventModule::dispatch(uint32_t module_id, uint32_t event, ArgsPtr args, CallbackFunc callback /* = nullptr */)
{
	EventInfo *_einfo = new EventInfo();
	_einfo->event_ = event;
	_einfo->target_ = module_id;

	if (callback) {
		_einfo->callback_ = callback;
	}

	_einfo->ptr_ = std::move(args);
	event_queue_.push(_einfo);
}

void gsf::EventModule::boardcast(uint32_t event, ArgsPtr args)
{
	for (auto &it : type_map_)
	{
		dispatch(it.first, event, std::move(args));
	}
}

void gsf::EventModule::bind_rpc(RpcFunc rpc_callback)
{
	rpc_ = rpc_callback;
}

void gsf::EventModule::dispatch_rpc(uint32_t event, int32_t moduleid, ArgsPtr args, RpcCallback callback /* = nullptr */)
{
	assert(rpc_);
	rpc_(event, moduleid, std::move(args), callback);
}

void gsf::EventModule::rmv_event(ModuleID module_id)
{
	auto tItr = type_map_.find(module_id);

	if (tItr != type_map_.end()) {
		tItr->second.clear();
	}
}

void gsf::EventModule::rmv_event(ModuleID module_id, EventID event_id)
{
	auto tItr = type_map_.find(module_id);

	if (tItr != type_map_.end()) {
	
		auto listItr = tItr->second;
		auto fItr = std::find_if(listItr.begin(), listItr.end(), [&](MIList::value_type it) {
			return (it.event_id_ == event_id);
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

void gsf::IEvent::rpc_listen(RpcFunc rpc_callback)
{
	EventModule::get_ref().bind_rpc(rpc_callback);
}

void gsf::IEvent::listen(Module *self, gsf::EventID event, DispatchFunc func)
{
	EventModule::get_ref().bind_event(self->get_module_id(), event, func);
}

void gsf::IEvent::listen(ModuleID self, gsf::EventID event, DispatchFunc func)
{
	EventModule::get_ref().bind_event(self, event, func);
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
	EventModule::get_ref().dispatch_rpc(event, moduleid, std::move(args), callback);
}

void gsf::IEvent::wipeout(ModuleID self)
{
	EventModule::get_ref().rmv_event(self);
}

void gsf::IEvent::wipeout(Module *self, EventID event_id)
{
	EventModule::get_ref().rmv_event(self->get_module_id(), event_id);
}

void gsf::IEvent::wipeout(Module *self)
{
	EventModule::get_ref().rmv_event(self->get_module_id());
}

void gsf::IEvent::wipeout(ModuleID self, EventID event_id)
{
	EventModule::get_ref().rmv_event(self, event_id);
}

