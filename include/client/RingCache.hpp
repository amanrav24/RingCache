#ifndef RING_CACHE_HPP
#define RING_CACHE_HPP

#include <vector>
#include <string>
#include <fstream>

#include "client/ConsistentHashing.hpp"
#include "server/NodeInfo.hpp"
#include "conf/nodeConfig.hpp"
#include "net/TcpClient.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

template <typename key, typename value>
class ringCache {
public:
    //Constructors
    ringCache(const vector<nodeConfig>& nodes);
    ~ringCache();

    //Operators
    bool cacheGet(const key& Key, value& Value);

    bool cacheSet(const key& Key, const value& Value);

    bool cacheDelete(const key& Key);

private:
    std::unordered_map<std::string, nodeConfig> configs;
    std::unordered_map<std::string, std::unique_ptr<tcpClient>> clients;
    ConsistentHashingRing hashRing;

    struct parseResult {
        bool success;
        std::string val; //optional val.
    };

    parseResult parseResponse(std::string response);

    // int sendRequestToNode(const NodeInfo& node, 
    //                       const string& op,
    //                       const key& k,
    //                       const value& value,
    //                       int ttl,
    //                       Value& result);
};



#endif