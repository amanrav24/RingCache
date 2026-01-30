#include "server/CacheNodeServer.hpp"

CacheNodeServer::CacheNodeServer(std::string nodeId, int port): info(nodeId), server(port) {

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
    server.sendResponse(response);
}

std::string CacheNodeServer::handleCommand(std::string result) {
    std::string command, key, value, response;

    json j = json::parse(result);
    json resMesg;

    command = j['op'];

    if (command == "set") {
        key = j['key'];
        value = j['value'];
        
        bool res = info.nodeSet(key, value, steadyClock::now() + chrono::seconds(5));
        
        if (res) {
            resMesg["status"] = "success";
        } else {
            resMesg["status"] = "failed";
        }
    } else if (command == "get") {
        key = j['key'];
       
        bool res = info.nodeGet(key, value);
        if (res) {
            resMesg['status'] = "success";
            resMesg['value'] = value;
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
