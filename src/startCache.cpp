#include <client/RingCache.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>

// Configuration for the Load Test
const int NUM_THREADS = 4;           
const int REQUESTS_PER_THREAD = 2500;
const bool CONTINUOUS_MODE = false;  


std::atomic<int> total_ops{0};

/**
 * Generates a random string to use as a key.
 */
std::string randomKey(int length = 5) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string s;
    s.reserve(length);
    for (int i = 0; i < length; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return "key_" + s;
}

void clientWorker(std::vector<nodeConfig> cluster, int id) {
    // Each thread gets its own client to ensure thread-safety on the socket layer
    ringCache<std::string, int> client(cluster);
    
    // Seed random number generator differently for each thread
    srand(time(NULL) + id); 

    int ops = 0;
    while (CONTINUOUS_MODE || ops < REQUESTS_PER_THREAD) {
        std::string key = randomKey();
        int value = rand() % 1000;
        int result_val = -1;

        // 1. SET Operation
        bool setRes = client.cacheSet(key, value);
        
        // 2. GET Operation
        bool getRes = client.cacheGet(key, result_val);

        // Verification (Optional: disable for raw speed)
        if (setRes && getRes && result_val != value) {
            std::cerr << "[Thread " << id << "] Data Mismatch! Key: " << key << std::endl;
        }

        ops++;
        total_ops++;
    }
}

int main(int argc, char * argv[]) {
    // --- 1. Setup Cluster Configuration (Same as before) ---
    const char* env = std::getenv("NODE_LIST");
    if (!env) {
        std::cerr << "NODE_LIST environment variable not found." << std::endl;
        return 1;
    }
    std::string nodeList(env);

    std::vector<std::string> nodes;
    std::stringstream ss(nodeList);
    std::string item;

    while (std::getline(ss, item, ',')) {
        nodes.push_back(item);
    }
    
    std::vector<nodeConfig> cluster;
    for (size_t i = 0; i < nodes.size(); i++) {
        size_t loc = nodes[i].find(":");
        std::string nodeId = nodes[i].substr(0, loc);
        std::string ipAddr = nodeId; // Docker DNS resolution
        int port = std::stoi(nodes[i].substr(loc + 1));
        cluster.push_back({nodeId, ipAddr, port});
    }

    std::cout << "--- Starting Distributed Load Test ---" << std::endl;
    std::cout << "Nodes: " << cluster.size() << std::endl;
    std::cout << "Threads: " << NUM_THREADS << std::endl;
    std::cout << "Requests Per Thread: " << REQUESTS_PER_THREAD << std::endl;
    std::cout << "Target Total Requests: " << (NUM_THREADS * REQUESTS_PER_THREAD * 2) << " (Set + Get)" << std::endl;

    // --- 2. Start Timer ---
    auto start_time = std::chrono::high_resolution_clock::now();

    // --- 3. Launch Workers ---
    std::vector<std::thread> workers;
    for (int i = 0; i < NUM_THREADS; i++) {
        workers.emplace_back(clientWorker, cluster, i);
    }

    // --- 4. Wait for Completion ---
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }

    // --- 5. Calculate Metrics ---
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;
    
    // Total operations = Sets + Gets
    int actual_ops = total_ops * 2; 

    std::cout << "\n--- Load Test Complete ---" << std::endl;
    std::cout << "Time Elapsed: " << diff.count() << " s" << std::endl;
    std::cout << "Total Operations: " << actual_ops << std::endl;
    std::cout << "Throughput: " << (actual_ops / diff.count()) << " Requests/Sec" << std::endl;

    return 0;
}




