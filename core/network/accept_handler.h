#ifndef _ACCEPT_HANDLER_HEADER_
#define _ACCEPT_HANDLER_HEADER_

namespace gsf 
{
    class AcceptHandler
    {
    public:
        virtual ~AcceptHandler(){}

        /**!
         *
         * */
        virtual void handler_new_connection(int acceptor_id, int session_id) = 0;
    };
}

#endif
