#ifndef _GSF_MODULE_HEADER_
#define _GSF_MODULE_HEADER_

#include <stdint.h>
#include <istream>

#include <functional>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <string>

#include "args.h"
#include "types.h"

#define WATCH_PERF

namespace gsf
{
	typedef std::function<void(ArgsPtr)> CallbackFunc;
	typedef std::function<void(gsf::ModuleID target, gsf::ArgsPtr)> ListenFunc;

	typedef std::function<void(ArgsPtr, int32_t, bool)> RpcCallback;

	class Module;
	struct TaskInfo
	{
		gsf::ModuleID target_;
		gsf::EventID event_;
		gsf::ArgsPtr args_;
	};

	/*

	*/
	struct MailBox
	{
		MailBox(Module *ptr);

		/*!

		**/
		void listen(gsf::EventID event, ListenFunc func);

		/*!

		**/
		void dispatch(gsf::ModuleID target, gsf::EventID event, gsf::ArgsPtr args);

		/*!

		**/
		void rpc(gsf::EventID event, ArgsPtr args, RpcCallback callback = nullptr);

		void pull();
		void push(TaskInfo *info);

	private:

		typedef std::queue<TaskInfo*> TaskQueue;

		std::unordered_map<gsf::EventID, ListenFunc> listenMap_;
		TaskQueue taskQueue_;

		Module *basePtr_ = nullptr;
	};

	typedef std::shared_ptr<MailBox> MailBoxPtr;

	class Module
	{
		friend class Application;
		friend class Reactor;

	public:
		Module(const std::string &name);
		virtual ~Module();

		gsf::ModuleID getModuleID() { return module_id_; }
		std::string & getModuleName() { return name_; }

		void listen(gsf::EventID event, ListenFunc func);
		void dispatch(gsf::ModuleID target, gsf::EventID event, gsf::ArgsPtr args);
		void rpc(gsf::EventID event, ArgsPtr args, RpcCallback callback = nullptr);

	protected:
		virtual void before_init();
		virtual void init();

		virtual void execute();

		virtual void shut();
		virtual void after_shut();

	protected:
		void setAvailable(bool flag) { available_ = flag; }
		bool getAvailable() const { return available_; }

		void setID(gsf::ModuleID id) { module_id_ = id; }
		gsf::ModuleID module_id_ = gsf::ModuleNil;
		std::string name_;

		bool available_ = false;

#ifdef WATCH_PERF
		int64_t tick_consume_;
		void add_tick_consume(int64_t c) { tick_consume_ += c; }

		virtual std::string get_tick_info(uint32_t count, uint32_t tick_count) 
		{
			auto c = static_cast<float>(tick_consume_ / 1000 / count);
			char buf[20];
			snprintf(buf, sizeof(buf), "%.3f", c);
#ifdef WIN32
			sscanf_s(buf, "%f", &c);
#else
			sscanf(buf, "%f", &c);
#endif
			std::string _info = getModuleName() + ":" + (buf) + " ms\t";
			tick_consume_ = 0;
			return _info;
		}
#endif // WATCH_PERF

		//! 
	private:
		MailBoxPtr mailboxPtr_ = nullptr;
	};
}


#endif
