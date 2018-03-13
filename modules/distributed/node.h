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
			int64_t id_ = 0;
		};
		typedef std::shared_ptr<CallbackInfo> CallbackPtr;

		struct ModuleInfo
		{
			std::string moduleName = "";
			gsf::ModuleID moduleID = gsf::ModuleNil;
			int32_t characteristic = 0;
		};

		struct NodeInfo
		{
			gsf::ModuleID connecotr_m_ = gsf::ModuleNil;
			std::string ip_ = "";
			int port_ = 0;
			int event_ = 0;
			gsf::ModuleID base_ = gsf::ModuleNil;
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

			void eventRpc(gsf::EventID event, gsf::ModuleID moduleID, const gsf::ArgsPtr &args, gsf::RpcCallback callback);

			void registNode(gsf::ModuleID base, int event, const std::string &ip, int port);

			void eCreateNode(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void eRegistNode(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

		private:

			gsf::ModuleID logM_ = gsf::ModuleNil;
			gsf::ModuleID timerM_ = gsf::ModuleNil;
			gsf::SessionID connectorFD_ = gsf::SessionNil;

			std::string acceptorIP_ = "";
			int32_t acceptorPort_ = 0;

			int32_t id_ = 0;
			std::string type_ = "";

			int32_t rpcDelay_ = 10000;
			bool service_ = false;
			gsf::ModuleID targetM_ = gsf::ModuleNil;

			std::vector<ModuleInfo> modules_;

			std::map<int64_t, CallbackPtr> callbackMap_;
			std::map<uint64_t, CallbackPtr> timerSet_;

			std::map<int, NodeInfo> eventMap_;
		};

	}
}


#endif