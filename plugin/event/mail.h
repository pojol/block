#ifndef _GSF_MAIL_HEADER_
#define _GSF_MAIL_HEADER_

#include <stdint.h>
#include <functional>
#include <unordered_map>

#include "event.h"

namespace gsf
{
    namespace core
    {
        typedef std::function<void(uint32_t, EventHandlerPtr)> EventFunc;

        class Mail
        {
        public:
            void regist(uint32_t event, EventFunc func);


            void dispatch(uint32_t event, int args, EventHandlerPtr callback = nullptr);

            void listen_result(Event *event, std::function<void(uint32_t event, uint32_t errcode)> callback);

        private:

            std::unordered_map<uint32_t, EventFunc> map_;
			std::unordered_map<uint32_t, std::function<void(uint32_t, uint32_t)>> self_map_;
        };
    }
}

#endif
