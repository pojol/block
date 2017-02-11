#include <stdlib.h>
#include <stdint.h>
#include <functional>

#include <iostream>
#include <memory>
#include <unordered_map>

#include <single.h>
#include <stream/ostream.h>
#include <stream/istream.h>
#include <network/message.h>

#include "event_handler.h"

class Event
{
public:
	Event()
	{
		//make event id
		std::cout << "make" << std::endl;
	}

	uint32_t event_id_;
};

class EventPlugin : public gsf::utils::Singleton<EventPlugin>
{
public:

	void dispatch(uint32_t event, EventHandlerPtr event_ptr)
	{
		// transport to the timer plugin

		auto itr = map_.find(event);
		if (itr != map_.end()){
			itr->second(event_ptr);
		}
	}

	//::utils::Args arg
	void regist_callback(Event *self, std::function<void(uint32_t, gsf::stream::IStream)> callback)
	{
		auto _block_ptr = std::make_shared<gsf::stream::Block>(4);
		_block_ptr->size_ = 4;

		gsf::stream::IStream is(_block_ptr, 0, 4);
		callback(1002, is);
	}

	void regist(uint32_t event, std::function<void (EventHandlerPtr)> func)
	{
		// check

		map_.insert(std::make_pair(event, func));
	}

private:
	
	std::unordered_map<uint32_t, std::function<void(EventHandlerPtr)>> map_;
};

class TimerPlugin : public gsf::utils::Singleton<TimerPlugin>
{
public:

	void init()
	{
		EventPlugin::get_ref().regist(1001, std::bind(&TimerPlugin::delay_milliseconds, this, std::placeholders::_1));
	}

	void delay_milliseconds(EventHandlerPtr event_ptr)
	{
		//EventPlugin::get_ref().dispatch(, )

		// test
		event_ptr->execute();
	}

};

class TestTimer : public Event
{
public:

	void init()
	{
		uint32_t event_delay_milliseconds = 1001;

		uint32_t callback_timer_event_regist_success = 1002;	// args timer_event_id
		uint32_t callback_timer_event_regist_fail = 1003;		// args timer_event_errcode

		EventPlugin::get_ref().regist_callback(this, [=](uint32_t event, gsf::stream::IStream is){
			if (event == callback_timer_event_regist_success) {
				uint32_t timer_event_id = 0;
				is >> timer_event_id;

				std::cout << "success by event id " << timer_event_id << std::endl;
			}
			else if (event == callback_timer_event_regist_fail) {
				uint32_t timer_event_errcode = 0;
				is >> timer_event_errcode;

				std::cout << "fail by errcode " << timer_event_errcode << std::endl;
			}
		});
		
		EventPlugin::get_ref().dispatch(event_delay_milliseconds, make_event(&TestTimer::click, this, 10));
	}

	void click(int i)
	{
		std::cout << "click " << i << std::endl;
	}

	~TestTimer()
	{

	}

};


int main()
{
	new TimerPlugin;
	TimerPlugin::get_ref().init();

	new EventPlugin;

	TestTimer t;
	t.init();

	system("pause");
	return 0;
}
