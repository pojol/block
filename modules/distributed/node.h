#ifndef _GSF_DISTRIBUTED_NODE_HEADER_
#define _GSF_DISTRIBUTED_NODE_HEADER_

#include <core/module.h>
#include <core/event.h>

namespace gsf
{
	namespace modules
	{

		class NodeModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:

			NodeModule();
			virtual ~NodeModule();

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

			
		private:

			void event_create_node(const gsf::ArgsPtr &args, gsf::CallbackFunc callback);

		private:

		};

	}
}


#endif