//
// Created by pojol on 2017/2/13.
//

#ifndef _GSF_EVENT_HEADER_
#define _GSF_EVENT_HEADER_

#include "../../common/single.h"

#include "../module/module.h"
#include "../stream/ostream.h"
#include "event_handler.h"

#include <functional>
#include <unordered_map>

namespace gsf
{
    namespace core
    {
        class Event
        {
        public:

        };

		typedef std::function<void(uint32_t, gsf::stream::OStream)> EventFunc;

		class Door
		{
		public:
			virtual void listen(Door *self, EventFunc func){}

			virtual void listen(uint32_t event, EventFunc func) {}

			virtual void dispatch(uint32_t event, gsf::stream::OStream args, EventHandlerPtr callback = nullptr){}
		};

		class EventModule
				: public Module
				, public gsf::utils::Singleton<EventModule>
		{
		public:


		protected:
			void execute();

        private:

		};
    }
}

#endif