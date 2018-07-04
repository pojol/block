#include "rpc.hpp"


#include <algorithm>
#include <iostream>
#include <utils/timer.hpp>

#ifdef WIN32
#include <winsock.h>
#endif // WIN32


block::modules::RPCModule::RPCModule()
	: Module("RPCModule")
{

}

block::modules::RPCModule::~RPCModule()
{

}

void block::modules::RPCModule::before_init()
{
    using namespace std::placeholders;

    listen(event::rpc_subscription, std::bind(&RPCModule::eSubscription, this, _1, _2));

    listen(event::rpc, std::bind(&RPCModule::eRpc, this, _1, _2));

    timeout_ = APP.getAppCfg().rpc_timeout;
    redisIp_ = APP.getAppCfg().rpcIp;
    redisPort_ = APP.getAppCfg().rpcPort;
    maxlen_ = APP.getAppCfg().rpc_maxlen;

    struct timeval _timeout = { 1, timeout_ };

    redis_context_ = redisConnectWithTimeout(redisIp_.c_str(), redisPort_, _timeout);
    if (redis_context_ && redis_context_->err == 0) {
        INFO_LOG("[BLOCK] rpc redis init succ!");
    }
    else {
        if (redis_context_) {
            ERROR_FMTLOG("[BLOCK] rpc service terminated! can't connect! err : {}", redis_context_->err);
        }
        else {
            ERROR_LOG("[BLOCK] rpc redis connect fail!");
        }   
    }
}

void block::modules::RPCModule::init()
{
}

void block::modules::RPCModule::execute()
{
}

void block::modules::RPCModule::shut()
{
}

bool block::modules::RPCModule::checkConnect()
{
    auto replyPtr = command(redis_context_, "PING");

    if (replyPtr->check("ping")) {
        return true;
    }
    else {
        WARN_LOG("[BLOCK] rpc connect fail! try reconnect!");

        struct timeval _timeout = { 1, timeout_ };

        redisFree(redis_context_);

        redis_context_ = redisConnectWithTimeout(redisIp_.c_str(), redisPort_, _timeout);
        if (redis_context_ && redis_context_->err == 0) {
            return true;
        }
        else {
            if (redis_context_){
                ERROR_FMTLOG("[BLOCK] rpc service terminated! can't connect! err : {}", redis_context_->err);
            }
            else {
                ERROR_LOG("[BLOCK] rpc service terminated! can't connect!");
            }
            return false;
        }
    }
}

block::modules::RpcReplyPtr block::modules::RPCModule::command(redisContext *c, const char *format, ...)
{
    va_list ap;
    void *reply = NULL;
    va_start(ap, format);
    reply = redisvCommand(c, format, ap);
    va_end(ap);

    return std::make_shared<RpcRedisReply>(static_cast<redisReply*>(reply));
}

void block::modules::RPCModule::eSubscription(block::ModuleID target, block::ArgsPtr args)
{

}

void block::modules::RPCModule::eRpc(block::ModuleID target, block::ArgsPtr args)
{
    std::string _channelName = args->pop_string();
    uint32_t _event = args->pop_ui32();

    if (!checkConnect()){
        dispatch(target, _event, block::makeArgs(1, false, "disconnect!"));
        return;
    }

    std::string _type = args->pop_string();
    uint32_t _page = args->pop_ui32();

    auto _replyPtr = command(redis_context_, "XADD " + _channelName + "* %d %s %d", _event, _type.c_str(), _page);
    if (!_reply->check("xadd")) {
        WARN_FMTLOG("[BLOCK] rpc command fail, {}", "xadd");
        dispatch(target, _event, block::makeArgs(1, false, "disconnect!"));
        return;
    }

    
}   