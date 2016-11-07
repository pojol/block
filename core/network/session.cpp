#include "session.h"


gsf::Session::Session(int32_t id)
    : id_(id)
{}

void gsf::Session::read_cb(::bufferevent *bev, void *ctx)
{

}

void gsf::Session::write_cb(::bufferevent *bev, void *ctx)
{

}

void gsf::Session::err_cb(::bufferevent *bev, short what, void *ctx)
{

}
