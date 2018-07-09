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
    // pipeline

    // observer

    // subscriber
}

void block::modules::RPCModule::shut()
{
}

bool block::modules::RPCModule::checkConnect()
{
    auto _replyPtr = command(redis_context_, "PING");
    int _status = 0;

    do {
        
        if (_replyPtr->type != REDIS_REPLY_ERROR){
            break;
        }

        WARN_LOG("[BLOCK] rpc connect fail! try reconnect!");
        redisFree(redis_context_);

        struct timeval _timeout = { 1, timeout_ };
        redis_context_ = redisConnectWithTimeout(redisIp_.c_str(), redisPort_, _timeout);
        if (redis_context_ && redis_context_->err == 0) {
            _status = 1;
            break;
        }
        else {
            if (redis_context_){
                ERROR_FMTLOG("[BLOCK] rpc service terminated! can't connect! err : {}", redis_context_->err);
                _status = -1;
            }
            else {
                ERROR_LOG("[BLOCK] rpc service terminated! can't connect!");
                _status = -2;
            }
        }

    }while(0);

    freeReplyObject(_replyPtr);

    if (_status < 0){
        return false;
    }
    else {
        return true;
    }
}

redisReply * block::modules::RPCModule::command(redisContext *c, const char *format, ...)
{
    va_list ap;
    void *reply = nullptr;
    va_start(ap, format);
    reply = redisvCommand(c, format, ap);
    va_end(ap);

    return static_cast<redisReply*>(reply);
}

void block::modules::RPCModule::eSubscription(block::ModuleID target, block::ArgsPtr args)
{
    // xrange

    // for xdel

    // transport
}

void block::modules::RPCModule::eRpc(block::ModuleID target, block::ArgsPtr args)
{
    // copy buf
    std::string _subscription = args->pop_string();
    uint32_t _event = args->pop_ui32();

    if (!checkConnect()){
        dispatch(target, _event, block::makeArgs(1, false, "disconnect!"));
        return;
    }

    std::string _type = args->pop_string();
    uint32_t _page = args->pop_ui32();
    // push pipeline

    std::string _cmd = "";
    _cmd += "XADD ";
    _cmd += _subscription;
    _cmd += "* %d %s %d";
    auto _replyPtr = command(redis_context_, _cmd.c_str(), _event, _type.c_str(), _page);   //use args buf
    if (_replyPtr->type == REDIS_REPLY_ERROR){
        WARN_FMTLOG("[BLOCK] rpc command fail, {}", "xadd");
        dispatch(target, _event, block::makeArgs(1, false, "command fail!"));
        freeReplyObject(_replyPtr);

        return;
    }
    freeReplyObject(_replyPtr);

    auto itr = std::find_if(observerVec_.begin(), observerVec_.end(), [&](ObserverVec::value_type it) {
		return (it.first == std::to_string(target));
	});

    if (itr != observerVec_.end()){
        itr->second.push_back("msg_id");
    }
    else {
        std::vector<std::string > _msgs;
        _msgs.push_back("msg_id");
        observerVec_.push_back(std::make_pair(std::to_string(target), _msgs));
    }
}   