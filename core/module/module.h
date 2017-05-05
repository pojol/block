#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

#include <stdint.h>
#include <istream>
#include <vector>
#include <string>

// ºóÃæ·Åµ½Í³Ò»µÄµØ·½
#define WATCH_PERF

namespace gsf
{
	class Module
	{
		friend class Application;
	public:
		Module(const std::string &name);
		virtual ~Module();

		uint32_t get_module_id() { return module_id_; }
		std::string & get_module_name() { return name_; }

	protected:
		virtual void before_init();
		virtual void init();

		virtual void execute();

		virtual void shut();
		virtual void after_shut();

	protected:
		void set_id(uint32_t id) { module_id_ = id; }
		uint32_t module_id_;
		std::string name_;

#ifdef WATCH_PERF
		double tick_consume_;
		void add_tick_consume(double c) { tick_consume_ += c; }

		std::string get_tick_info(uint32_t count, uint32_t tick_count) 
		{
			auto c = tick_consume_ / 1000 / count;
			char buf[20];
			sprintf(buf, "%.3f", c);
			sscanf(buf, "%f", &c);

			std::string _info = get_module_name() + ":" + (buf) + " ms\t";
			tick_consume_ = 0;
			return _info;
		}
#endif // WATCH_PERF

	};
}


#endif
