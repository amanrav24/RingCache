#include <client/RingCache.hpp>

template <typename key, typename value>
ringCache<key, value>::ringCache(const vector<nodeConfig>& nodes) {
    for (size_t i = 0; i < nodes.size(); i++) {
        hashRing.addNode(nodes[i].nodeId);
        configs[nodes[i].nodeId] = nodes[i];
        clients[nodes[i].nodeId] = tcpClient(nodes[i].nodeId, nodes[i].port);
    }
}


template <typename key, typename value>
typename ringCache<key, value>::parseResult
ringCache<key, value>::parseResponse(std::string response) {
    parseResult res;
    json resMesg = json::parse(response);

    if (resMesg["status"] == "failed") {
        res.success = false;
        res.val = "";
        return res;
    } else {
        res.success = true;
        
        if (res.val != "") {
            res.success = resMesg["value"];
        }
    }

    return res;
}

template <typename key, typename value>
bool ringCache<key, value>::cacheGet(const key& Key, value& Value) {
    string nodeId = hashRing.getNode(Key);

    if (nodeId == "") {
        return false;
    }

    tcpClient curClient = clients[nodeId];
    bool connectionRes = curClient.connectToServer();

    if (!connectionRes) {
        return false;
    }

    json mesg;
    mesg["op"] = "get";
    mesg["key"] =  Key;

    std::string mesgStr = mesg.dump() + "\n";

    bool sendRes = curClient.sendAll(mesgStr);

    if (!sendRes) {
        return false;
    }

    std::string result = curClient.receiveLine();
    parseResult mesgRes = parseResponse(result);

    if (!mesgRes.success) {
        return false;
    }

    Value = mesgRes.val;

    return true;
}

template <typename key, typename value>
bool ringCache<key, value>::cacheSet(const key& Key, const value& Value) {
    string nodeId = hashRing.getNode(Key);

    if (nodeId == "") {
        return false;
    }

    tcpClient curClient = clients[nodeId];
    bool connectionRes = curClient.connectToServer();

    if (!connectionRes) {
        return false;
    }

    json mesg;
    mesg["op"] = "set";
    mesg["key"] =  Key;
    mesg["value"] = Value;

    std::string mesgStr = mesg.dump() + "\n";

    bool sendRes = curClient.sendAll(mesgStr);

    if (!sendRes) {
        return false;
    }

    std::string result = curClient.receiveLine();
    parseResult mesgRes = parseResponse(result);

    if (!mesgRes.success) {
        return false;
    }
    
    return true;
}

template <typename key, typename value>
bool ringCache<key, value>::cacheDelete(const key& Key) {
    string nodeId = hashRing.getNode(Key);

    if (nodeId == "") {
        return false;
    }

    tcpClient curClient = clients[nodeId];
    bool connectionRes = curClient.connectToServer();

    if (!connectionRes) {
        return false;
    }

    json mesg;
    mesg["op"] = "del";
    mesg["key"] =  Key;

    std::string mesgStr = mesg.dump() + "\n";

    bool sendRes = curClient.sendAll(mesgStr);

    if (!sendRes) {
        return false;
    }

    std::string result = curClient.receiveLine();
    parseResult mesgRes = parseResponse(result);

    if (!mesgRes) {
        return false;
    }
    
    return true;
}
