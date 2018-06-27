#pragma once

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
			std::string moduleName_ = "";
			block::ModuleID moduleID_ = block::ModuleNil;
		};
 
		struct NodeInfo
		{
			std::vector<ModuleInfo> modules;

			int32_t nodeID_ = 0;
			std::string nodeType_ = "";
			std::string nodeIP_ = "";
			int nodePort_ = 0;

			block::ModuleID connector_m_ = block::ModuleNil;

			int32_t weight_ = 0;
		};
		typedef std::shared_ptr<NodeInfo> NodePtr;

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

			void eNodeInit(block::ModuleID target, block::ArgsPtr args);
			void eNodeRpc(block::ModuleID target, block::ArgsPtr args);

		private:
			block::SessionID connector_fd_ = block::SessionNil;
			block::ModuleID connector_m_ = block::ModuleNil;
			
			int32_t nodeID_ = 0;
			std::string nodeType_ = "";
			std::string nodeIp_ = "";
			int32_t nodePort_ = 0;
			std::string rootIP_ = "";
			int32_t rootPort_ = 0;
			std::vector<std::pair<block::ModuleID, std::string>> registModules_;
			

			int32_t rpcDelay_ = 10000;

			std::map<int64_t, CallbackPtr> callbackMap_;
			std::map<uint64_t, CallbackPtr> timerSet_;

			std::map<int32_t, NodePtr> nodeMap_;
			std::multimap<std::string, NodePtr> moduleMap_;
		};

	}
}
