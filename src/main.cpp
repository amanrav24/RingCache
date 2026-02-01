#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>

#include <iostream>

int main() {
    std::cout << "Program Start\n";

    //create three nodes
    std::vector<nodeConfig> cluster = {
        {"node-0", "127.0.0.1", 8080},
        // {"node-1", "127.0.0.1", 8081},
        // {"node-2", "127.0.0.1", 8082},
        // {"node-3", "127.0.0.1", 8083},
        // {"node-4", "127.0.0.1", 8084}   
    };

    std::cout << "Done with cluster" << std::endl;

    ringCache<std::string, int> testCache(cluster);

    std::cout << "testCache" << std::endl;

    //dont like this, but cant do move stuff with tcpclass...
    //std::vector<std::unique_ptr<CacheNodeServer>> nodeServers;

    // for (int i = 0; i < cluster.size(); i++) {
    //     nodeServers.push_back(
    //         std::make_unique<CacheNodeServer>(cluster[i].nodeId, cluster[i].port)
    //     );
    // }

    // for (int i = 0; i < cluster.size(); i++) {
    //     auto& node = nodeServers[i];
    //     node -> start();
    // }

    int res1 = -1;
    int res2 = -1;
    int res3 = -1;

    testCache.cacheSet("t1", 1);
    // testCache.cacheSet("t2", 2);
    // testCache.cacheSet("t3", 3);

    testCache.cacheGet("t1", res1);
    // testCache.cacheGet("t2", res2);
    // testCache.cacheGet("t3", res3);

    //std::cout << "This is res: " << res1 << std::endl;


    std::cout << "This is res: " << res1 << " " << res2 << " " << res3 << " " << std::endl;

    return 0;
}

