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

    listen(event::rpc_regist_consumer, std::bind(&RPCModule::eRegistConsumer, this, _1, _2));
    listen(event::rpc_regist_consumer_group, std::bind(&RPCModule::eRegistConsumerGroup, this, _1, _2));

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
    if (!checkConnect()){
        return;
    }

    redisReply *_replyPtr = nullptr;

    // pipeline
    if (reidsPipelineCount_ > 0) {

        for (int32_t i = 0; i < reidsPipelineCount_; ++i)
        {
            if (REDIS_OK != redisGetReply(redis_context_, (void**)&_replyPtr)) {
                ERROR_FMTLOG("[BLOCK] rpc consume pipeline fail, err : {}", _replyPtr->str);
            }

            freeReplyObject(_replyPtr);
            _replyPtr = nullptr;
        }
    }

    for (auto itr = consumerList_.begin(); itr != consumerList_.end(); ++itr)
    {
        std::string _cmd = "";
        _cmd += "XRANGE ";
        _cmd += *itr;
        _cmd += " - +";
        _replyPtr = command(redis_context_, _cmd.c_str());
        if (_replyPtr->type != REDIS_REPLY_ERROR){
            consume(*itr, _replyPtr);
        }
        else {
            ERROR_FMTLOG("[BLOCK] rpc xrange fail, err : {}", _replyPtr->str);
        }

        freeReplyObject(_replyPtr);
        _replyPtr = nullptr;
    }
}

void block::modules::RPCModule::consume(const std::string &moduleName, redisReply *replyPtr)
{
    for (int i = 0; i < replyPtr->elements; ++i)
    {
        auto cmd = replyPtr->element[i];

        std::string _cmdid = "";
        int32_t _event = 0;
        std::string _buf = "";

        for (int j = 0; j < cmd->elements; ++j)
        {
            auto context = cmd->element[j];
            if (context->type != REDIS_REPLY_ARRAY){    // id
                _cmdid = context->str;
            }
            else {
                auto key = context->element[0];
                _event = std::stoi(key->str);

                auto val = context->element[1];
                _buf = val->str;
            }
        }

        // dispatch
        auto _smartPtr = block::ArgsPool::get_ref().get();
        _smartPtr->importBuf(_buf);
        dispatch(APP.getModuleID(moduleName), _event, std::move(_smartPtr));

        std::string _cmd = "";
        _cmd.append("XDEL ");
        _cmd.append(moduleName);
        _cmd.append(" ");
        _cmd.append(_cmdid);
        auto _replyPtr = command(redis_context_, _cmd.c_str()); 
        if (_replyPtr->type == REDIS_REPLY_ERROR) {
            ERROR_FMTLOG("[BLOCK] rpc consume fail, err : {}", _replyPtr->str);
        }
        freeReplyObject(_replyPtr);
    }
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

    } while(0);

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

void block::modules::RPCModule::eRegistConsumer(block::ModuleID targetr, block::ArgsPtr args)
{
    if (!checkConnect()){
        WARN_LOG("[BLOCK] rpc regist consumer fail, connect failed!");
        return;
    }

    auto _moduleName = args->pop_string();

    std::string _cmd = "";
    _cmd += "EXISTS ";
    _cmd += _moduleName;

    auto _replyPtr = command(redis_context_, _cmd.c_str());
    if (_replyPtr->integer != 0){
        WARN_LOG("[BLOCK] rpc regist consumer fail, repeat regist!");
        freeReplyObject(_replyPtr);
        return;
    }
    freeReplyObject(_replyPtr);

    auto _itr = std::find_if(consumerList_.begin(), consumerList_.end(), [&](ConsumerList::value_type it){
        return (it == _moduleName);
    });
    assert(_itr == consumerList_.end());

    // 
    consumerList_.push_back(_moduleName);
}

void block::modules::RPCModule::eRegistConsumerGroup(block::ModuleID targetr, block::ArgsPtr args)
{
}

void block::modules::RPCModule::eRpc(block::ModuleID target, block::ArgsPtr args)
{
    // copy buf
    std::string _buf = args->exportBuf();

    std::string _consumer = args->pop_string();
    int32_t _event = args->pop_ui32();

    if (!checkConnect()){
        dispatch(target, _event, block::makeArgs(1, false, "disconnect!"));
        return;
    }

    std::string _cmd = "";
    _cmd += "XADD ";
    _cmd += _consumer;
    _cmd += " * %s %b";

    if (REDIS_OK != redisAppendCommand(redis_context_, _cmd.c_str(), std::to_string(_event).c_str(), _buf.c_str(), _buf.length())){
        ERROR_FMTLOG("[BLOCK] rpc xadd fail, consumer : {}", _consumer);
    }
    else {
        reidsPipelineCount_++;
    }
}