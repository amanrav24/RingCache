#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>

#include <iostream>

// This has its OWN main function
int main(int argc, char * argv[]) {
    //std::cout << "--- Starting Single Node Server Test ---" << std::endl;

    nodeConfig config = {.nodeId = argv[1], .ipAddress = "127.0.0.1", .port = stoi(argv[2])};
        
    // Example: Instantiate your server directly here
    // (Adjust arguments based on your actual constructor)
    try {
        int port = stoi(argv[2]);
        std::cout << "Initializing server on port " << port << "..." << std::endl;

        std::unique_ptr<CacheNodeServer> node = std::make_unique<CacheNodeServer>(config.nodeId, config.port);
        node->start();
        std::cout << "Server test executed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}