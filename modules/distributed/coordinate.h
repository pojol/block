#ifndef _COORDINATE_HEADER_
#define _COORDINATE_HEADER_

#include <core/module.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace block
{
	namespace modules
	{
		struct CModuleInfo
		{
			block::ModuleID module_id_;
			std::string module_name_;
			int32_t module_characteristic_;
		};

		struct CNodeInfo
		{
			std::vector<CModuleInfo> modules;

			int32_t nod_id = 0;
			std::string type_ = "";
			uint32_t weight_ = 0;

			std::string acceptor_ip_ = "";
			int32_t acceptor_port_ = 0;
		};
		typedef std::shared_ptr<CNodeInfo> NodePtr;


		/**
		coodinator 协调多个相同业务app之间的负载
		*/
		class CoodinatorModule
			: public block::Module
		{
		public:

			CoodinatorModule();
			virtual ~CoodinatorModule() {}

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		private:
			void eCoordinatAdjustWeight(block::ModuleID target, block::ArgsPtr args);
			void eCoordinatSelect(block::ModuleID target, block::ArgsPtr args);
			void eCoordinatRegist(block::ModuleID target, block::ArgsPtr args);
			void eCoordinatUnregist(block::ModuleID target, block::ArgsPtr args);

			void adjustModuleWeight(int32_t nod_id, const std::string &module_name, block::ModuleID module_id, int32_t characteristic, int32_t weight);

		private:

			std::map<block::SessionID, NodePtr> nodeIDMap_;
			std::multimap<std::string, NodePtr> nodeNameMap_;
		};
	}
}


#endif // !_COORDINATE_HEADER_
