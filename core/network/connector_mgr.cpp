#include "connector_mgr.h"
#include "connector.h"
#include "err.h"

gsf::ConnectorMgr* gsf::ConnectorMgr::instance_ = nullptr;

gsf::ConnectorMgr::ConnectorMgr()
        :connector_index_(0)
{

}

gsf::ConnectorMgr::~ConnectorMgr()
{

}

gsf::ConnectorMgr& gsf::ConnectorMgr::instance()
{
    if (instance_ == nullptr)
    {
        instance_ = new gsf::ConnectorMgr();
    }
    return *instance_;
}

int gsf::ConnectorMgr::make_connector(const ConnectorConfig &config)
{
    connector_index_++;

    auto _acceptor_ptr = std::make_shared<Connector>(connector_index_, config);
    if (!_acceptor_ptr){
        return CONNECTOR_CONSTRUCT_ERR;
    }

    connector_vec_.push_back(_acceptor_ptr);

    return connector_index_;
}

int gsf::ConnectorMgr::open(int connector_id, ConnectorHandler *connector_handler)
{
    return 0;
}

int gsf::ConnectorMgr::close(int connector_id)
{
    return 0;
}

gsf::ConnectorPtr gsf::ConnectorMgr::find_connector(int connector_id)
{
    auto itr = std::find_if(connector_vec_.begin(), connector_vec_.end(), [&](ConnectorVec::value_type it){
        return (it->get_id() == connector_id);
    });
    if (itr != connector_vec_.end()){
        return (*itr);
    }
    else {
        return nullptr;
    }
}