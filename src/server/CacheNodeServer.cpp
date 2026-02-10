#include "server/CacheNodeServer.hpp"
#include "metrics/MetricsRegistry.hpp" 
#include <iostream>
#include <chrono>              

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
    // Start the latency timer (Microseconds)
    auto start = std::chrono::high_resolution_clock::now();

    // Increment total requests received
    MetricsRegistry::getInstance().incrementRequest();

    // read from the clientFd
    std::string result = server.readLine(clientFd);
    std::string response = handleCommand(result);
    server.sendResponse(response, clientFd);

    // Stop timer and record latency
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    MetricsRegistry::getInstance().recordLatency(duration);
}

std::string CacheNodeServer::handleCommand(std::string result) {
    std::string command, key, value, response;

    // Use try-catch for robust parsing (Metrics shouldn't crash on bad JSON)
    try {
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

            // Note: Eviction tracking belongs inside NodeInfo or the Local Cache 
            // where the actual storage limit is checked.
            bool res = info.nodeSet(key, value, steadyClock::now() + chrono::seconds(5));
            
            if (res) {
                resMesg["status"] = "success";
            } else {
                resMesg["status"] = "failed";
            }
        } else if (command == "get") {
            bool res = info.nodeGet(key, value);
            
            if (res) {
                // Record Cache Hit
                MetricsRegistry::getInstance().incrementHit();
                
                resMesg["status"] = "success";
                resMesg["value"] = value;
            } else {
                // Record Cache Miss
                MetricsRegistry::getInstance().incrementMiss();
                
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
    } 
    catch (const std::exception& e) {
        // Fallback for malformed requests
        std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        return "{\"status\":\"error\", \"message\":\"invalid_json\"}\n";
    }

    return response;
}
