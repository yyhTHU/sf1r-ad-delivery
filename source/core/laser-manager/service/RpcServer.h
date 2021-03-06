#ifndef RPC_SERVER_H_
#define RPC_SERVER_H_

#include <3rdparty/msgpack/rpc/server.h>
#include "TermParser.h"
#include "CLUSTERINGServerRequest.h"
#include "TermParser.h"
#include "laser-manager/clusteringmanager/type/ClusteringDataAdapter.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
namespace sf1r 
{ 
namespace laser 
{
class RpcServer : public msgpack::rpc::server::base
{
public:
    static boost::shared_ptr<RpcServer>& getInstance();
    ~RpcServer();

private:
    RpcServer();
public:
    void start(const std::string& host, 
        int port, 
        int threadNum, 
        const std::string& clusteringRootPath, 
        const std::string& clusteringDBPath, 
        const std::string& perUserDBPath);
    void stop();
    
    virtual void dispatch(msgpack::rpc::request req);
private:
    bool init(const std::string& clusteringRootPath,const std::string& clusteringDBPath,const std::string& pcaPath);

private:
    bool isStarted_;
    boost::shared_mutex mutex_;
};

} //namespace laser
} //namespace sf1r

#endif /* RPC_SERVER_H_ */
