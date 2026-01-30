#include <server/NodeInfo.hpp>


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
    m.lock();
    if (store.find(key) != store.end()) {
        store.erase(key);
    }

    if (expiration.find(key) != expiration.end()) {
        expiration.erase(key);
    }
    m.unlock();

    return true;
}


bool NodeInfo::nodeGet(const std::string& key, std::string& value) {
    //Already expired/removed
    m.lock();
    if (store.find(key) == store.end()) {
        return false;
    }

    if (expiration.find(key) != expiration.end() && expiration[key] <= steadyClock::now()) {
        expiration.erase(key);
        store.erase(key);
        return false;
    }

    value = store[key];
    m.unlock();

    return true;
}


bool NodeInfo::nodeSet(const std::string& key, const std::string& value, timePoint exp) {
    m.lock();
    store[key] = value;
    expiration[key] = exp;
    
    cacheEntry newEntry;
    newEntry.expirationTime = exp;
    newEntry.key = key;

    timeToLive.push(newEntry);
    m.unlock();

    return true;
}  

void NodeInfo::backgroundCleanup() {
    while (!stopRequested) {
        queueCleanup();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return;
}

void NodeInfo::queueCleanup() {
    auto timeNow = steadyClock::now();
    
    m.lock();
    while (!stopRequested && !timeToLive.empty()) {
        const cacheEntry& topEntry = timeToLive.top();

        if (topEntry.expirationTime > timeNow) {
            break;
        }

        if (!stopRequested) {
            auto it = expiration.find(topEntry.key);

            if (!stopRequested && it != expiration.end()) {
                store.erase(topEntry.key);
                expiration.erase(topEntry.key);
            }
        }   
        
        timeToLive.pop();
    }
    m.unlock();
}


