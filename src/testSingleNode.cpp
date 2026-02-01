#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>

#include <iostream>

// This has its OWN main function
int main() {
    std::cout << "--- Starting Single Node Server Test ---" << std::endl;

    std::vector<nodeConfig> cluster = {
        {"node-0", "127.0.0.1", 8080},
        // {"node-1", "127.0.0.1", 8081},
        // {"node-2", "127.0.0.1", 8082},
        // {"node-3", "127.0.0.1", 8083},
        // {"node-4", "127.0.0.1", 8084}   
    };

    // Example: Instantiate your server directly here
    // (Adjust arguments based on your actual constructor)
    try {
        int port = 8080;
        std::cout << "Initializing server on port " << port << "..." << std::endl;

        std::unique_ptr<CacheNodeServer> node = std::make_unique<CacheNodeServer>(cluster[0].nodeId, cluster[0].port);
        node ->start();
        std::cout << "Server test executed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}