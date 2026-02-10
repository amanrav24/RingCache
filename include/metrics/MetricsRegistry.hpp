#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

/**
 * MetricsRegistry
 * Singleton class to store global system metrics using thread-safe atomics.
 * * "Zero-Dependency" Observability:
 * Uses std::atomic to ensure that multiple worker threads can increment stats
 * without locking a mutex.
 */
class MetricsRegistry {
public:
    // Singleton Access
    static MetricsRegistry& getInstance() {
        static MetricsRegistry instance; 
        return instance;
    }

    MetricsRegistry(const MetricsRegistry&) = delete;
    MetricsRegistry& operator=(const MetricsRegistry&) = delete;


    // Metrics Actions (The "Write" API)
    void incrementRequest() {
        requests_total.fetch_add(1, std::memory_order_relaxed);
    }

    void incrementHit() {
        hits.fetch_add(1, std::memory_order_relaxed);
    }

    void incrementMiss() {
        misses.fetch_add(1, std::memory_order_relaxed);
    }

    void incrementEviction() {
        evictions.fetch_add(1, std::memory_order_relaxed);
    }

    void incrementKeysStored() {
        keys_stored.fetch_add(1, std::memory_order_relaxed);
    }
    
    void decrementKeysStored() {
        keys_stored.fetch_sub(1, std::memory_order_relaxed);
    }

    /**
     * Sorts a request duration into a histogram bucket.
     * Buckets:
     * [0]: < 1ms       (Excellent)
     * [1]: 1ms - 5ms   (Good)
     * [2]: 5ms - 10ms  (Acceptable)
     * [3]: 10ms - 50ms (Slow)
     * [4]: > 50ms      (Critical/Stall)
     * * @param microseconds Duration of the request in microseconds
     */
    void recordLatency(long microseconds) {
        if (microseconds < 1000) {
            latency_buckets[0].fetch_add(1, std::memory_order_relaxed);
        } else if (microseconds < 5000) {
            latency_buckets[1].fetch_add(1, std::memory_order_relaxed);
        } else if (microseconds < 10000) {
            latency_buckets[2].fetch_add(1, std::memory_order_relaxed);
        } else if (microseconds < 50000) {
            latency_buckets[3].fetch_add(1, std::memory_order_relaxed);
        } else {
            latency_buckets[4].fetch_add(1, std::memory_order_relaxed);
        }
    }

    
    // Data Access (The "Read" API)

    uint64_t getRequestsTotal() const { return requests_total.load(std::memory_order_relaxed); }
    uint64_t getHits() const { return hits.load(std::memory_order_relaxed); }
    uint64_t getMisses() const { return misses.load(std::memory_order_relaxed); }
    uint64_t getEvictions() const { return evictions.load(std::memory_order_relaxed); }
    uint64_t getKeysStored() const { return keys_stored.load(std::memory_order_relaxed); }

    // Helper to read a specific bucket (0-4)
    uint64_t getLatencyBucket(int index) const {
        if (index < 0 || index >= 5) return 0;
        return latency_buckets[index].load(std::memory_order_relaxed);
    }

private:
    // Private constructor initializes all atomics to 0
    MetricsRegistry() : 
        requests_total(0), 
        hits(0), 
        misses(0), 
        evictions(0), 
        keys_stored(0) 
    {
        // Explicitly initialize array elements
        for(int i=0; i<5; ++i) {
            latency_buckets[i].store(0);
        }
    }


    // Internal State
    std::atomic<uint64_t> requests_total;
    std::atomic<uint64_t> hits;
    std::atomic<uint64_t> misses;
    std::atomic<uint64_t> evictions;
    std::atomic<uint64_t> keys_stored;

    // Fixed-size array for histogram buckets
    // std::atomic is not copyable, so we use a raw C-array which is valid inside a non-copyable class
    std::atomic<uint64_t> latency_buckets[5];
};