#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>

#include <iostream>

int main(int argc, char * argv[]) {
    std::cout << "Program Start\n";
    int portStart = 8080;

    //create three nodes
    std::vector<nodeConfig> cluster;

    for (size_t i = 0; i < (argc - 1); i++) {
        std::string nodeId = "NODE" + std::to_string(i);
        std::string ipAdd = "127.0.0.1";
        int port = portStart + i;
        cluster.push_back(
            {nodeId, ipAdd, port}
        );
    }

    ringCache<std::string, int> testCache(cluster);

    std::cout << "Cache Configured" << std::endl;

    //Cache test actions below
    int res1 = -1;
    int res2 = -1;
    int res3 = -1;


    testCache.cacheSet("t1", 1);
    testCache.cacheSet("t2", 2);
    testCache.cacheSet("t3", 3);

    testCache.cacheGet("t1", res1);
    testCache.cacheGet("t2", res2);
    testCache.cacheGet("t3", res3);

    std::cout << "This is res: " << res1 << " " << res2 << " " << res3 << " " << std::endl;

    return 0;
}




