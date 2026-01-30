#ifndef NODE_INFO_HPP
#define NODE_INFO_HPP

#include <unordered_map>
#include <string>
#include <chrono>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>


using namespace std;
using steadyClock = chrono::steady_clock;
using timePoint = steadyClock::time_point;

class NodeInfo {
public:
    NodeInfo(const string& id);
    ~NodeInfo();

    bool nodeSet(const std::string& key, const std::string& value, timePoint exp);
    bool nodeGet(const std::string& key, std::string& value);
    bool nodeDelete(const std::string& key);

    void queueCleanup();

    string getId() const;

private:
    string nodeId;
    unordered_map<std::string, std::string> store;
    unordered_map<std::string, timePoint> expiration;

    std::mutex m;
    std::atomic<bool> stopRequested{false};
    std::thread cleanupThread;

    void backgroundCleanup();

    struct cacheEntry {
        std::string key;
        timePoint expirationTime;
    };

    struct entryCompare {
        bool operator()(const cacheEntry& a, const cacheEntry& b) const {
            return a.expirationTime > b.expirationTime;
        }
    };

    priority_queue<cacheEntry, vector<cacheEntry>, entryCompare> timeToLive; 
};


#endif 