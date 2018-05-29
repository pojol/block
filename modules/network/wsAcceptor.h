#pragma once

#include <core/module.h>
#include <unordered_map>


namespace block 
{

	namespace network
	{
		class WebSocketAcceptorModule
			: public block::Module
		{
		public:
			WebSocketAcceptorModule(const std::string &name);
			WebSocketAcceptorModule();
			virtual ~WebSocketAcceptorModule();

			void before_init() override;
			void init() override;

			void execute() override;

			void shut() override;
			void after_shut() override;

		private:

			void eMakeAcceptor(block::ModuleID target, block::ArgsPtr args);

		};

	}

}