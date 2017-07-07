#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>

#include <iostream>
#include <random>
#include <tuple>

#include <core/application.h>
#include <core/event.h>

#include <timer/timer.h>
#include <log/log.h>

struct Case_DelayMilliseconds
{
	void test()
	{
		create_timer_(2000, [&](const gsf::ArgsPtr &args) {
			timer_id_ = args->pop_ui64();
			print_f_(gsf::make_log(gsf::LogInfo, "Case_DelayMilliseconds", "regist success! timer id = ", timer_id_));
		});
	}

	void on_timer()
	{
		print_f_(gsf::make_log(gsf::LogInfo, "Case_DelayMilliseconds", "arrive! timer id = ", timer_id_));
		timer_id_ = gsf::TimerNil;
	}

	std::function<void(const gsf::ArgsPtr &args)> print_f_;
	std::function<void(int32_t, gsf::CallbackFunc cb)> create_timer_;

	gsf::TimerID timer_id_;
	
};

struct Case_DelayDay
{
	void test()
	{
		create_timer_(6, 0, [&](const gsf::ArgsPtr &args) {
			timer_id_ = args->pop_ui64();
			print_f_(gsf::make_log(gsf::LogInfo, "Case_DelayDay", "regist success! timer id = ", timer_id_));
		});
	}

	void on_timer()
	{
		print_f_(gsf::make_log(gsf::LogInfo, "Case_DelayDay", "arrive! timer id = ", timer_id_));
		timer_id_ = gsf::TimerNil;
	}

	std::function<void(const gsf::ArgsPtr &args)> print_f_;
	std::function<void(int32_t, int32_t, gsf::CallbackFunc cb)> create_timer_;

	gsf::TimerID timer_id_;
};

struct Case_RemoveTimer
{

	void test()
	{
		create_timer_(1000, [&](const gsf::ArgsPtr &args) {
			timer_id_ = args->pop_ui64();
			print_f_(gsf::make_log(gsf::LogInfo, "Case_RemoveTimer", "regist success! timer id = ", timer_id_));
		});

		remove_timer_(timer_id_, [&](const gsf::ArgsPtr &args) {
			print_f_(gsf::make_log(gsf::LogInfo, "Case_RemoveTimer", "remove ret = ", args->pop_i32(), " timer id = ", timer_id_));
		});
	}

	std::function<void(const gsf::ArgsPtr &args)> print_f_;
	std::function<void(int32_t, gsf::CallbackFunc cb)> create_timer_;
	std::function<void(gsf::TimerID, gsf::CallbackFunc cb)> remove_timer_;

	gsf::TimerID timer_id_;
};


struct TestCaseModule
	: public gsf::Module
	, public gsf::IEvent
{
	TestCaseModule()
		: Module("TestCaseModule")
	{}

	virtual ~TestCaseModule() {}

	void before_init() override
	{
		dispatch(eid::app_id, eid::get_module, gsf::make_args("TimerModule"), [&](const gsf::ArgsPtr &args) {
			timer_m_ = args->pop_i32();
		});

		dispatch(eid::app_id, eid::get_module, gsf::make_args("LogModule"), [&](const gsf::ArgsPtr &args) {
			log_m_ = args->pop_i32();
		});
	}

	void init() override
	{
		auto _delay_millisecoinds = [&](int32_t delay, gsf::CallbackFunc cb) {
			dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(get_module_id(), delay), cb);
		};

		auto _delay_day = [&](int32_t hour, int32_t min, gsf::CallbackFunc cb) {
			dispatch(timer_m_, eid::timer::delay_day, gsf::make_args(get_module_id(), hour, min), cb);
		};

		auto _remove_timer = [&](uint64_t timer_id, gsf::CallbackFunc cb) {
			dispatch(timer_m_, eid::timer::remove_timer, gsf::make_args(get_module_id(), timer_id), cb);
		};

		auto _print = [&](const gsf::ArgsPtr &args) {
			dispatch(log_m_, eid::log::print, args);
		};

		case_delaymillseconds_.create_timer_ = _delay_millisecoinds;
		case_delaymillseconds_.print_f_ = _print;
		case_delayday_.create_timer_ = _delay_day;
		case_delayday_.print_f_ = _print;
		case_remove_timer_.create_timer_ = _delay_millisecoinds;
		case_remove_timer_.remove_timer_ = _remove_timer;
		case_remove_timer_.print_f_ = _print;

		listen(this, eid::timer::timer_arrive, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc cb) {

			auto _timer_id = args->pop_ui64();
			if (_timer_id == case_delaymillseconds_.timer_id_) {
				case_delaymillseconds_.on_timer();
			}
			if (_timer_id == case_delayday_.timer_id_) {
				case_delayday_.on_timer();
			}
		});

		case_delaymillseconds_.test();
		case_delayday_.test();
		case_remove_timer_.test();
	}

private:
	uint32_t timer_m_;
	uint32_t log_m_;

	Case_DelayMilliseconds case_delaymillseconds_;
	Case_DelayDay case_delayday_;
	Case_RemoveTimer case_remove_timer_;
};