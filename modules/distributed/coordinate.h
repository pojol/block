#ifndef _COORDINATE_HEADER_
#define _COORDINATE_HEADER_

#include <core/module.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace gsf
{
	namespace modules
	{
		struct CModuleInfo
		{
			gsf::ModuleID module_id_;
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
			: public gsf::Module
		{
		public:

			CoodinatorModule();
			virtual ~CoodinatorModule() {}

			void before_init() override;


		private:
			void eCoordinatAdjustWeight(gsf::ModuleID target, gsf::ArgsPtr args);
			void eCoordinatSelect(gsf::ModuleID target, gsf::ArgsPtr args);
			void eCoordinatRegist(gsf::ModuleID target, gsf::ArgsPtr args);
			void eCoordinatUnregist(gsf::ModuleID target, gsf::ArgsPtr args);

			void adjustModuleWeight(int32_t nod_id, const std::string &module_name, gsf::ModuleID module_id, int32_t characteristic, int32_t weight);

		private:

			std::map<gsf::SessionID, NodePtr> nodeIDMap_;
			std::multimap<std::string, NodePtr> nodeNameMap_;
		};
	}
}


#endif // !_COORDINATE_HEADER_
