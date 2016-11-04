#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_


namespace gsf
{
	//! 管理buffer调整水位， 处理读写事件
	class Session
	{
    public:
        static void read_cb(struct bufferevent *bev, void *ctx);
        static void write_cb(struct bufferevent *bev, void *ctx);

    private:
	};
}


#endif
