#ifndef _GSF_DISTRIBUTED_NODE_HEADER_
#define _GSF_DISTRIBUTED_NODE_HEADER_

#include <core/module.h>
#include <core/event.h>

#include <vector>
#include <tuple>
#include <functional>

namespace gsf
{
	namespace modules
	{
		struct ConnInfo
		{
			gsf::ModuleID m_ = gsf::ModuleNil;
			std::string name_ = "";
			std::string ip_ = "";
			int32_t port_ = 0;
		};

		class NodeModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:

			NodeModule();
			virtual ~NodeModule();

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

			
		private:

			void event_create_node(const gsf::ArgsPtr &args, gsf::CallbackFunc callback);

		private:

			gsf::ModuleID log_m_ = gsf::ModuleNil;

			gsf::ModuleID acceptor_m_ = gsf::ModuleNil;
			
			ConnInfo accept_info_;
			std::vector<ConnInfo> conn_vec_;
		};

	}
}


#endif