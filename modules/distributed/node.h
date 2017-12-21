#ifndef _GSF_DISTRIBUTED_NODE_HEADER_
#define _GSF_DISTRIBUTED_NODE_HEADER_

#include <core/module.h>
#include <core/event.h>

#include <vector>
#include <map>
#include <tuple>
#include <functional>

namespace gsf
{
	namespace modules
	{
		struct CallbackInfo
		{
			gsf::RpcCallback callback;
			uint64_t timer_ = 0;
			uint32_t event_ = 0;
		};
		typedef std::shared_ptr<CallbackInfo> CallbackPtr;

		struct ModuleInfo
		{
			std::string moduleName = "";
			gsf::ModuleID moduleID = gsf::ModuleNil;
			int32_t characteristic = 0;
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

		protected:

			void event_rpc(const std::string &module, int event, const gsf::ArgsPtr &args, gsf::RpcCallback callback);

			gsf::ArgsPtr event_create_node(const gsf::ArgsPtr &args);

		private:

			gsf::ModuleID log_m_ = gsf::ModuleNil;
			gsf::ModuleID timer_m_ = gsf::ModuleNil;
			gsf::ModuleID connector_m_ = gsf::ModuleNil;
			gsf::SessionID connector_fd_ = gsf::SessionNil;

			std::string root_ip_ = "";
			int32_t root_port_ = 0;

			std::string acceptor_ip_ = "";
			int32_t acceptor_port_ = 0;

			int32_t id_ = 0;
			std::string type_ = "";

			int32_t rpc_delay_ = 3000;
			bool service_ = false;
			gsf::ModuleID target_m_ = gsf::ModuleNil;

			std::vector<ModuleInfo> modules_;

			std::map<uint32_t, CallbackPtr> callback_map_;
			std::map<uint64_t, CallbackPtr> timer_set_;
		};

	}
}


#endif