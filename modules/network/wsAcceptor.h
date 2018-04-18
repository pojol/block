#pragma once

#include <core/module.h>
#include <unordered_map>


namespace gsf 
{

	namespace network
	{
		class WebSocketAcceptorModule
			: public gsf::Module
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

			void eMakeAcceptor(gsf::ModuleID target, gsf::ArgsPtr args);

		};

	}

}