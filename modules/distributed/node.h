#ifndef _GSF_DISTRIBUTED_NODE_HEADER_
#define _GSF_DISTRIBUTED_NODE_HEADER_

#include <core/module.h>

#include <vector>
#include <map>
#include <tuple>
#include <functional>

namespace block
{
	namespace modules
	{
		struct CallbackInfo
		{
			block::RpcCallback callback;
			uint64_t timer_ = 0;
			uint32_t event_ = 0;
			int64_t id_ = 0;
		};
		typedef std::shared_ptr<CallbackInfo> CallbackPtr;

		struct ModuleInfo
		{
			std::string moduleName = "";
			block::ModuleID moduleID = block::ModuleNil;
			int32_t characteristic = 0;
		};

		struct NodeInfo
		{
			block::ModuleID connecotr_m_ = block::ModuleNil;
			std::string ip_ = "";
			int port_ = 0;
			int event_ = 0;
			block::ModuleID base_ = block::ModuleNil;
		};

		class NodeModule
			: public block::Module
		{
		public:

			NodeModule();
			virtual ~NodeModule();

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		protected:

			void eventRpc(block::EventID event, block::ModuleID moduleID, const block::ArgsPtr &args, block::RpcCallback callback);

			void registNode(block::ModuleID base, int event, const std::string &ip, int port);

			void eCreateNode(block::ModuleID target, block::ArgsPtr args);
			void eRegistNode(block::ModuleID target, block::ArgsPtr args);

		private:

			block::ModuleID logM_ = block::ModuleNil;
			block::ModuleID timerM_ = block::ModuleNil;
			block::SessionID connectorFD_ = block::SessionNil;

			std::string acceptorIP_ = "";
			int32_t acceptorPort_ = 0;

			int32_t delayTag_ = 1;

			int32_t id_ = 0;
			std::string type_ = "";

			int32_t rpcDelay_ = 10000;
			bool service_ = false;
			block::ModuleID targetM_ = block::ModuleNil;

			std::vector<ModuleInfo> modules_;

			std::map<int64_t, CallbackPtr> callbackMap_;
			std::map<uint64_t, CallbackPtr> timerSet_;

			std::map<int, NodeInfo> eventMap_;
		};

	}
}


#endif