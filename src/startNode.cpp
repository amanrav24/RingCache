#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>
#include <metrics/MetricsServer.hpp> 

#include <iostream>
#include <cstdlib>
#include <thread>                   

int main(int argc, char * argv[]) {
    //std::cout << "--- Starting Single Node Server Test ---" << std::endl;
    std::string nodeId = std::getenv("NODE_ID");
    int port = std::stoi(std::getenv("PORT"));
    std::string ipAddr = std::getenv("NODE_IP");


    nodeConfig config = {.nodeId = nodeId, .ipAddress = ipAddr, .port = port};
        
    try {
        std::cout << "Initializing server on port " << port << "..." << std::endl;

        // Launch the Metrics Server in a background thread
        std::thread metricsThread(startMetricsServer, 9090);
        metricsThread.detach(); // Detach so it runs independently of the main server loop

        std::unique_ptr<CacheNodeServer> node = std::make_unique<CacheNodeServer>(config.nodeId, config.port);
        node->start();
        std::cout << "Server test executed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}