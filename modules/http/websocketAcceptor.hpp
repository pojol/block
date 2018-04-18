#pragma once

#include <core/module.h>
#include <unordered_map>

#include "http_server.hpp"


namespace gsf 
{

	namespace network
	{
		struct WSSession
		{
			int64_t tick_;
			
			int32_t fd_;
			
			//!
			std::weak_ptr<cinatra::connection<cinatra::Socket>> connPtr_;

		};

		typedef std::shared_ptr<WSSession> WSSessionPtr;

		class WebSocketAcceptorModule
			: public gsf::Module
		{
		public:
			WebSocketAcceptorModule(const std::string &name);
			virtual ~WebSocketAcceptorModule();

			void before_init() override;
			void init() override;

			void execute() override;

			void shut() override;
			void after_shut() override;

		private:

			void eMakeAcceptor(gsf::ModuleID target, gsf::ArgsPtr args);


		private:
			cinatra::http_server_<cinatra::io_service_inplace> server_;
			bool isRuning_ = false;

			std::unordered_map<int64_t, WSSessionPtr> sessionMap_;
		};

	}

}