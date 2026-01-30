#pragma once

#include <sstream>

#include "server/NodeInfo.hpp"
#include "net/TcpServer.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class CacheNodeServer {
public:
    CacheNodeServer(std::string nodeId, int port);
    ~CacheNodeServer();

    void start();
private:
    tcpServer server;
    NodeInfo info;

    void clientHandler(int clientFd);
    std::string handleCommand(std::string result);
};