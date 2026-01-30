#ifndef CONSISTENT_HASHING
#define CONSISTENT_HASHING

#include <map>
#include <string>

using namespace std;

class ConsistentHashingRing {
public:
    void addNode(const string& nodeId);
    
    void removeNode(const string& nodeId);
  
    template <typename Key>
    string getNode(const Key& key) const {
        if(ring.empty()) {
            return "";
        }

        size_t keyHash = hash<string>{}(key);

        auto it = ring.lower_bound(keyHash);

        //Special Case: When key is greater than all keys, so assign to first key
        if (it == ring.end()) {
            return ring.begin() -> second;
        }

        return it -> second;
    }

private:
    map<size_t, string> ring;
    //implement virtual nodes later here
};



#endif