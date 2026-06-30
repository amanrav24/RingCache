#include <client/ConsistentHashing.hpp>

#include <functional>
#include <map>
#include <format>

void ConsistentHashingRing::addNode(const string &nodeId)
{
    for (int i = 0; i < virtual_nodes; i++)
    {
        std::string vNode = std::format("{}#{}", nodeId, i);
        size_t nodeHash = hash<std::string>{}(vNode);
        ring[nodeHash] = nodeId;
    }
}

void ConsistentHashingRing::removeNode(const string &nodeId)
{
    for (int i = 0; i < virtual_nodes; i++)
    {
        std::string vNode = std::format("{}#{}", nodeId, i);
        size_t nodeHash = hash<std::string>{}(vNode);
        ring.erase(nodeHash);
    }
}