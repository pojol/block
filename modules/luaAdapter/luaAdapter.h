#ifndef _GSF_LUA_PROXY_HEADER_
#define _GSF_LUA_PROXY_HEADER_

#include <sol.hpp>
#include <array>

#include <core/module.h>
#include <core/application.h>

#include <vector>
#include <string>

namespace gsf
{
	namespace modules
	{
		enum LuaAppState
		{
			BEFORE_INIT = 0,
			INIT,
			EXECUTE,
			SHUT,
			AFTER_SHUT,
		};

		struct LuaProxy
		{
			LuaProxy()
				: app_state_(LuaAppState::BEFORE_INIT)
			{}

			sol::state state_;
			std::string dir_name_;
			std::string file_name_;
			LuaAppState app_state_;

			std::array<std::function<void(sol::table)>, 5> call_list_;
		};

		typedef std::shared_ptr<LuaProxy> LuaProxyPtr;

		class LuaAdapterModule
			: public gsf::Module
		{

		public:
			LuaAdapterModule(const std::string &name)
				: Module(name)
			{}

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		private:
			//sol::variadic_args args

			//代理下event的dispatch 和 listen 接口，因为要在这里集中捕获下lua产生的异常。
			void ldispatch(gsf::ModuleID target, gsf::EventID event, const std::string &buf);

			int llisten(uint32_t event, const sol::function &func);

			void lrpc(uint32_t event, int32_t moduleid, const std::string &buf);

			//void eDistory(gsf::ModuleID target, gsf::ArgsPtr args);
			void eReload(gsf::ModuleID target, gsf::ArgsPtr args);

			void create();

			int destroy(uint32_t module_id);

			LuaProxyPtr proxyPtr_ = nullptr;

		protected:
			std::string dir_ = "";
			std::string name_ = "";

		private:

			bool reloading = false;

		};
	}
}

#endif
