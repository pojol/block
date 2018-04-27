#pragma once

#include <core/module.h>
#include <unordered_map>

namespace block 
{

	namespace network
	{
		class WebSocketConnectorModule
			: public block::Module
		{
		public:
			WebSocketConnectorModule(const std::string &name);
			WebSocketConnectorModule();
			virtual ~WebSocketConnectorModule();

			void before_init() override;
			void init() override;

			void execute() override;

			void shut() override;
			void after_shut() override;

		private:

			void eMakeConnector(block::ModuleID target, block::ArgsPtr args);
		};

	}

}