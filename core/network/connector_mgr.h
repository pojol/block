#ifndef _CONNECTOR_MANAGER_HEADER_
#define _CONNECTOR_MANAGER_HEADER_

#include <cstdint>
#include <memory>
#include <vector>

namespace gsf
{
    struct ConnectorConfig;
    class Connector;
    class ConnectorHandler;

    typedef std::shared_ptr<Connector> ConnectorPtr;

    class ConnectorMgr
    {
    public:
        ~ConnectorMgr();

        static ConnectorMgr & instance();

        int make_connector(const ConnectorConfig &config);

        int open(int connector_id, ConnectorHandler *connector_handler);

        int close(int connector_id);

        ConnectorPtr find_connector(int connector_id);

    protected:
        ConnectorMgr();
        static ConnectorMgr *instance_;

    private:
        typedef std::vector<ConnectorPtr> ConnectorVec;
        ConnectorVec connector_vec_;

        uint32_t connector_index_;
    };
}

#endif