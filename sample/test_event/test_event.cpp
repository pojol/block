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

	void dispatch(uint32_t event, uint32_t args, EventHandlerPtr event_ptr)
	{
		// transport to the timer plugin

		auto itr = map_.find(event);
		if (itr != map_.end()){
			itr->second(args, event_ptr);
		}
	}

	void dispatch2self(uint32_t event, gsf::stream::IStream is)
	{
		auto itr = self_map_.find(1);
		if (itr != self_map_.end()){
			itr->second(is);
		}
		//callback(1002, is);
	}

	//::utils::Args arg
	

	bool execute()
	{
		//while (list.end())
		//{
		//		pop
		//		execute
		//}

		return true;
	}

	void regist(uint32_t event, std::function<void (uint32_t, EventHandlerPtr)> func)
	{
		// check

		map_.insert(std::make_pair(event, func));
	}

	void listen_result(Event *self, std::function<void(gsf::stream::IStream)> callback)
	{
		self_map_.insert(std::make_pair(1, callback));

	}

private:
	
	std::unordered_map<uint32_t, std::function<void(uint32_t, EventHandlerPtr)>> map_;
	std::unordered_map<uint32_t, std::function<void(gsf::stream::IStream)>> self_map_;
};

class TimerPlugin : public gsf::utils::Singleton<TimerPlugin>
{
public:

	void init()
	{
		using namespace std::placeholders;
		EventPlugin::get_ref().regist(1001, std::bind(&TimerPlugin::delay_milliseconds, this, _1, _2));
	}

	bool execute()
	{
		return true;
	}

	void delay_milliseconds(uint32_t args, EventHandlerPtr event_ptr)
	{
		auto _block_ptr = std::make_shared<gsf::stream::Block>(4);
		_block_ptr->size_ = 4;

		gsf::stream::IStream is(_block_ptr, 0, 4);
		EventPlugin::get_ref().dispatch2self(1, is);

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

		uint32_t timer_event_regist_success = 1002;	// args timer_event_id
		uint32_t timer_event_regist_fail = 1003;		// args timer_event_errcode

		EventPlugin::get_ref().listen_result(this, [=](gsf::stream::IStream is){
			uint32_t arg1, arg2 = 0;
			is >> arg1;
			//is >> arg2;

			arg1 = 1002;

			if (arg1 == timer_event_regist_success) {
				std::cout << "success by event id " << arg2 << std::endl;
			}
			else if (arg1 == timer_event_regist_fail) {
				std::cout << "fail by errcode " << arg2 << std::endl;
			}
		});

		EventPlugin::get_ref().dispatch(event_delay_milliseconds, 10
			, make_callback(&TestTimer::click, this, std::string("hello,timer!")));
	}

	void click(std::string str)
	{
		std::cout << str << std::endl;
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
