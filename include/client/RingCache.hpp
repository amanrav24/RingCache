#ifndef RING_CACHE_HPP
#define RING_CACHE_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream> 

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
    ringCache(const vector<nodeConfig>& nodes) {
        for (size_t i = 0; i < nodes.size(); i++) {
            hashRing.addNode(nodes[i].nodeId);
            configs[nodes[i].nodeId] = nodes[i];
            clients[nodes[i].nodeId] = std::make_unique<tcpClient>(nodes[i].nodeId, nodes[i].port, nodes[i].ipAddress);
        }
    }

    ~ringCache() {}

    //Operators
    bool cacheGet(const key& Key, value& Value) {
        string nodeId = hashRing.getNode(Key);

        if (nodeId == "") {
            return false;
        }

        auto& curClient = clients[nodeId];
        bool connectionRes = curClient->connectToServer();

        if (!connectionRes) {
            return false;
        }

        json mesg;
        mesg["op"] = "get";
        mesg["key"] =  Key;

        std::string mesgStr = mesg.dump() + "\n";

        bool sendRes = curClient->sendAll(mesgStr);

        if (!sendRes) {
            return false;
        }

        std::string result = curClient->receiveLine();
        parseResult mesgRes = parseResponse(result);

        if (!mesgRes.success) {
            return false;
        }

        Value = mesgRes.val;

        return true;
    }

    bool cacheSet(const key& Key, const value& Value) {
        string nodeId = hashRing.getNode(Key);

        if (nodeId == "") {
            std::cout << "Failed to find node" << std::endl;
            return false;
        }

        auto& curClient = clients[nodeId];
        bool connectionRes = curClient->connectToServer();

        if (!connectionRes) {
            std::cout << "Failed to connect to server" << std::endl;
            return false;
        }

        json mesg;
        mesg["op"] = "set";
        mesg["key"] =  Key;
        mesg["value"] = Value;

        std::string mesgStr = mesg.dump() + "\n";

        bool sendRes = curClient->sendAll(mesgStr);

        if (!sendRes) {
            std::cout << "Failed to send" << std::endl;
            return false;
        }

        std::string result = curClient->receiveLine();
        parseResult mesgRes = parseResponse(result);

        if (!mesgRes.success) {
            std::cout << "Mesg failed" << std::endl;
            return false;
        }

        return true;
    }

    bool cacheDelete(const key& Key) {
        string nodeId = hashRing.getNode(Key);

        if (nodeId == "") {
            return false;
        }

        auto& curClient = clients[nodeId];
        bool connectionRes = curClient->connectToServer();

        if (!connectionRes) {
            return false;
        }

        json mesg;
        mesg["op"] = "del";
        mesg["key"] =  Key;

        std::string mesgStr = mesg.dump() + "\n";

        bool sendRes = curClient->sendAll(mesgStr);

        if (!sendRes) {
            return false;
        }

        std::string result = curClient->receiveLine();
        parseResult mesgRes = parseResponse(result);

        return mesgRes.success;
    }

private:
    std::unordered_map<std::string, nodeConfig> configs;
    std::unordered_map<std::string, std::unique_ptr<tcpClient>> clients;
    ConsistentHashingRing hashRing;

    struct parseResult {
        bool success;
        value val; //optional val.
        
        // Added implicit conversion to bool to support your logic in cacheDelete: if (!mesgRes)
        operator bool() const { return success; }
    };

    value fromString(const std::string& s) {
        if constexpr (std::is_same_v<value, std::string>) {
            return s; // If T is string, just return it.
        }
        std::stringstream ss(s);
        value result;
        ss >> result; // "Streams" the text into the type (handling parsing logic)
        return result;
    }

    parseResult parseResponse(std::string response) {
        std::cout << "This is resp: " << response << std::endl;
        parseResult res;
        json resMesg = json::parse(response);
        if (resMesg["status"] == "failed") {
            res.success = false;
            return res;
        } else {
            res.success = true;
            if (resMesg.contains("value")) {
                res.val = fromString(resMesg["value"].get<std::string>());
            }
        }

        return res;
    }
};

#endif