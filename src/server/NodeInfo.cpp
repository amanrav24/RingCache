#include "server/NodeInfo.hpp"
#include "metrics/MetricsRegistry.hpp"

NodeInfo::NodeInfo(const string& id) {
    nodeId = id;
    cleanupThread = std::thread(&NodeInfo::backgroundCleanup, this);
}

NodeInfo::~NodeInfo() {
    stopRequested = true;
    if (cleanupThread.joinable()) {
        cleanupThread.join();
    }
}

string NodeInfo::getId() const {
    return nodeId;
}

bool NodeInfo::nodeDelete(const std::string& key) {
    std::lock_guard<std::mutex> lock(m); 
    
    // Decrement if key exists
    if (store.find(key) != store.end()) {
        store.erase(key);
        MetricsRegistry::getInstance().decrementKeysStored();
    }

    if (expiration.find(key) != expiration.end()) {
        expiration.erase(key);
    }

    return true;
}


bool NodeInfo::nodeGet(const std::string& key, std::string& value) {
    std::lock_guard<std::mutex> lock(m);

    // Already expired/removed
    if (store.find(key) == store.end()) {
        return false;
    }

    // Check expiration on access (Lazy Expiration)
    if (expiration.find(key) != expiration.end() && expiration[key] <= steadyClock::now()) {
        expiration.erase(key);
        store.erase(key);
        
        // Decrement because it expired
        MetricsRegistry::getInstance().decrementKeysStored();
        return false;
    }

    value = store[key];
    return true;
}


bool NodeInfo::nodeSet(const std::string& key, const std::string& value, timePoint exp) {
    std::lock_guard<std::mutex> lock(m);
    
    // Only increment if this is a NEW key (not an update)
    if (store.find(key) == store.end()) {
        MetricsRegistry::getInstance().incrementKeysStored();
    }

    store[key] = value;
    expiration[key] = exp;
    
    cacheEntry newEntry;
    newEntry.expirationTime = exp;
    newEntry.key = key;

    timeToLive.push(newEntry);
    return true;
}  

void NodeInfo::backgroundCleanup() {
    while (!stopRequested) {
        queueCleanup();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void NodeInfo::queueCleanup() {
    auto timeNow = steadyClock::now();
    
    std::lock_guard<std::mutex> lock(m);
    
    while (!stopRequested && !timeToLive.empty()) {
        const cacheEntry& topEntry = timeToLive.top();

        if (topEntry.expirationTime > timeNow) {
            break;
        }

        // Double check expiration map to ensure it wasn't updated/removed elsewhere
        auto it = expiration.find(topEntry.key);

        if (!stopRequested && it != expiration.end()) {
            // Decrement on background expiration
            if (store.find(topEntry.key) != store.end()) {
                store.erase(topEntry.key);
                MetricsRegistry::getInstance().decrementKeysStored();
            }
            expiration.erase(topEntry.key);
        }   
        
        timeToLive.pop();
    }
}