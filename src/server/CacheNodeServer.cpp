#include "server/CacheNodeServer.hpp"

#include <iostream>

CacheNodeServer::CacheNodeServer(std::string nodeId, int port): server(port), info(nodeId) {

}

void CacheNodeServer::start() {
    server.start(
        [this](int clientFd) {
            clientHandler(clientFd);
        }
    );
}

void CacheNodeServer::clientHandler(int clientFd) {
    //read from the clientFd
    std::string result = server.readLine(clientFd);
    std::string response = handleCommand(result);
    server.sendResponse(response, clientFd);
}

std::string CacheNodeServer::handleCommand(std::string result) {
    std::string command, key, value, response;

    json j = json::parse(result);
    json resMesg;

    command = j["op"];

    if (j["key"].is_string()) {
        key = j["key"].get<std::string>();
    } else {
        key = j["key"].dump();
    }

    if (command == "set") {
        if (j["value"].is_string()) {
            value = j["value"].get<std::string>();
        } else {
            value = j["value"].dump();
        }

        bool res = info.nodeSet(key, value, steadyClock::now() + chrono::seconds(5));
        
        if (res) {
            resMesg["status"] = "success";
        } else {
            resMesg["status"] = "failed";
        }
    } else if (command == "get") {
        bool res = info.nodeGet(key, value);
        if (res) {
            resMesg["status"] = "success";
            resMesg["value"] = value;
        } else {
            resMesg["status"] = "failed";
        }
        
    } else if (command == "del") {
        bool res = info.nodeDelete(key);

        if (res) {
            resMesg["status"] = "success";
        } else {
            resMesg["status"] = "failed";
        }
    }

    response = resMesg.dump() + '\n';

    return response;
}
