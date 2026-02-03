#include <client/RingCache.hpp>
#include <server/CacheNodeServer.hpp>

#include <iostream>

int main(int argc, char * argv[]) {
    const char* env = std::getenv("NODE_LIST");
    std::string nodeList(env);

    std::vector<std::string> nodes;
    std::stringstream ss(nodeList);
    std::string item;

    while (std::getline(ss, item, ',')) {
        nodes.push_back(item);
    }
    
    //Formatted for the purpose of docker testing --> look at environment varibles
    //nodeId can be resolved to ipAddr within docker network
    std::vector<nodeConfig> cluster;
    for (size_t i = 0; i < nodes.size(); i++) {
        size_t loc = nodes[i].find(":");
        std::string nodeId = nodes[i].substr(0, loc);
        std::string ipAddr = nodeId;
        int port = std::stoi(nodes[i].substr(loc + 1));
        cluster.push_back(
            {nodeId, ipAddr, port}
        );
    }

    ringCache<std::string, int> testCache(cluster);

    std::cout << "Cache Configured" << std::endl;

    //Cache test actions below
    int res1 = -1;
    int res2 = -1;
    int res3 = -1;


    bool set1 = testCache.cacheSet("t1", 1);
    bool set2 = testCache.cacheSet("t2", 2);
    bool set3 = testCache.cacheSet("t3", 3);

    bool get1 = testCache.cacheGet("t1", res1);
    bool get2 = testCache.cacheGet("t2", res2);
    bool get3 = testCache.cacheGet("t3", res3);
    
    std::cout << "This is set: " << set1 << " " << set2 << " " << set3 << " " << std::endl;
    std::cout << "This is res: " << get1 << " " << get2 << " " << get3 << " " << std::endl;
    std::cout << "This is res: " << res1 << " " << res2 << " " << res3 << " " << std::endl;

    return 0;
}




