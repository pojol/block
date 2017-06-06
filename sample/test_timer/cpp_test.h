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

struct TestCaseModule;

struct Case_DelayMilliseconds
{
	void test()
	{
		module_ptr_->i_delay_milliseconds(2000, [&](gsf::Args args) {
			timer_id_ = args.pop_uint64(0);
			module_ptr_->log_f_(eid::log::info, "Case_DelayMilliseconds", gsf::Args("regist success! timer id = ", timer_id_));
		});
	}

	void on_timer()
	{
		module_ptr_->log_f_(eid::log::info, "Case_DelayMilliseconds", gsf::Args("arrive! timer id = ", timer_id_));
		timer_id_ = gsf::TimerNil;
	}

	gsf::TimerID timer_id_;
	TestCaseModule *module_ptr_;
};

struct Case_DelayDay
{
	void test()
	{
		module_ptr_->i_delay_day(6, 0, [&](gsf::Args args) {
			timer_id_ = args.pop_uint64(0);
			module_ptr_->log_f_(eid::log::info, "Case_DelayDay", gsf::Args("regist success! timer id = ", args.pop_uint64(0)));
		});
	}

	void on_timer()
	{
		module_ptr_->log_f_(eid::log::info, "Case_DelayDay", gsf::Args("arrive! timer id = ", timer_id_));
		timer_id_ = gsf::TimerNil;
	}

	gsf::TimerID timer_id_;
	TestCaseModule *module_ptr_;
};

struct Case_RemoveTimer
{
	void test()
	{
		module_ptr_->i_delay_milliseconds(1000, [&](gsf::Args args) {
			timer_id_ = args.pop_uint64(0);
			module_ptr_->log_f_(eid::log::info, "Case_RemoveTimer", gsf::Args("regist success! timer id = ", timer_id_));
		});

		module_ptr_->i_remove_timer(timer_id_, [&](gsf::Args args) {
			module_ptr_->log_f_(eid::log::info, "Case_RemoveTimer", gsf::Args("remove ret = ", args.pop_int32(0), " timer id = ", timer_id_));
		});
	}

	TestCaseModule *module_ptr_;
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

	void before_init()
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args("TimerModule"), [&](const gsf::Args &args) {
			timer_m_ = args.pop_int32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](const gsf::Args &args) {
			log_m_ = args.pop_int32(0);
		});

		dispatch(log_m_, eid::log::log_callback, gsf::Args(), [&](const gsf::Args &args) {
			log_f_ = args.pop_log_callback(0);
		});
	}

	void init()
	{
		case_delaymillseconds_.module_ptr_ = this;
		case_delayday_.module_ptr_ = this;
		case_remove_timer_.module_ptr_ = this;

		listen(this, eid::timer::timer_arrive, [&](gsf::Args args, gsf::CallbackFunc cb) {

			auto _timer_id = args.pop_uint64(0);
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

	void i_delay_milliseconds(int32_t delay, gsf::CallbackFunc cb)
	{
		dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), delay), cb);
	}

	void i_delay_day(int32_t hour, int32_t min, gsf::CallbackFunc cb)
	{
		dispatch(timer_m_, eid::timer::delay_day, gsf::Args(get_module_id(), hour, min), cb);
	}

	void i_remove_timer(uint64_t timer_id, gsf::CallbackFunc cb)
	{
		dispatch(timer_m_, eid::timer::remove_timer, gsf::Args(get_module_id(), timer_id), cb);
	}

	gsf::LogFunc log_f_;

private:
	uint32_t timer_m_;
	uint32_t log_m_;

	Case_DelayMilliseconds case_delaymillseconds_;
	Case_DelayDay case_delayday_;
	Case_RemoveTimer case_remove_timer_;
};