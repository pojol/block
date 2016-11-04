#ifndef _ACCEPT_HANDLER_HEADER_
#define _ACCEPT_HANDLER_HEADER_

namespace gsf 
{
    class AcceptHandler
    {
    public:
        virtual ~AcceptHandler(){}

        /**!
         * 建立连接成功！这个函数开始可以开启session的侦听，绑定message<T>
         * */
        virtual void handler_new_connection(int acceptor_id, int session_id) = 0;
    };
}

#endif
