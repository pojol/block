#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

#include <stdint.h>
#include <vector>

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
	};
}


#endif
