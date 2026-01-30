#include <client/ConsistentHashing.hpp>

#include <functional>
#include <map>

using namespace std;

void ConsistentHashingRing::addNode(const string& nodeId) {
    size_t nodeHash = hash<string>{}(nodeId);
    ring[nodeHash] = nodeId;
}

void ConsistentHashingRing::removeNode(const string& nodeId) {
    size_t nodeHash = hash<string>{}(nodeId);
    ring.erase(nodeHash);
}