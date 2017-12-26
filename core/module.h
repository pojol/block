#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

#include <stdint.h>
#include <istream>
#include <vector>
#include <string>
#include "types.h"

#define WATCH_PERF

namespace gsf
{
	class Module
	{
		friend class Application;
	public:
		Module(const std::string &name);
		virtual ~Module();

		gsf::ModuleID get_module_id() { return module_id_; }
		std::string & get_module_name() { return name_; }

	protected:
		virtual void before_init();
		virtual void init();

		virtual void execute();

		virtual void shut();
		virtual void after_shut();

	protected:
		void set_id(gsf::ModuleID id) { module_id_ = id; }
		gsf::ModuleID module_id_ = gsf::ModuleNil;
		std::string name_;

#ifdef WATCH_PERF
		int64_t tick_consume_;
		void add_tick_consume(int64_t c) { tick_consume_ += c; }

		virtual std::string get_tick_info(uint32_t count, uint32_t tick_count) 
		{
			auto c = static_cast<float>(tick_consume_ / 1000 / count);
			char buf[20];
			snprintf(buf, sizeof(buf), "%.3f", c);
#ifdef WIN32
			sscanf_s(buf, "%f", &c);
#else
			sscanf(buf, "%f", &c);
#endif
			std::string _info = get_module_name() + ":" + (buf) + " ms\t";
			tick_consume_ = 0;
			return _info;
		}
#endif // WATCH_PERF

	};
}


#endif
