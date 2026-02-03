#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>

#include <iostream>
#include <cstdlib>

// This has its OWN main function
int main(int argc, char * argv[]) {
    //std::cout << "--- Starting Single Node Server Test ---" << std::endl;
    std::string nodeId = std::getenv("NODE_ID");
    int port = std::stoi(std::getenv("PORT"));
    std::string ipAddr = std::getenv("NODE_IP");


    nodeConfig config = {.nodeId = nodeId, .ipAddress = ipAddr, .port = port};
        
    // Example: Instantiate your server directly here
    // (Adjust arguments based on your actual constructor)
    try {
        std::cout << "Initializing server on port " << port << "..." << std::endl;

        std::unique_ptr<CacheNodeServer> node = std::make_unique<CacheNodeServer>(config.nodeId, config.port);
        node->start();
        std::cout << "Server test executed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}