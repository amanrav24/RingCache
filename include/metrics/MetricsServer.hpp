#ifndef METRICS_SERVER_HPP
#define METRICS_SERVER_HPP

#include "MetricsRegistry.hpp"
#include <string>
#include <sstream>
#include <thread>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

/**
 * @brief Formats the current metrics into Prometheus exposition format.
 */
inline std::string generatePrometheusOutput() {
    std::stringstream ss;
    auto& metrics = MetricsRegistry::getInstance();

    // 1. HELP and TYPE headers for Prometheus
    ss << "# HELP ringcache_requests_total Total number of requests processed\n";
    ss << "# TYPE ringcache_requests_total counter\n";
    ss << "ringcache_requests_total " << metrics.getRequestsTotal() << "\n\n";

    ss << "# HELP ringcache_cache_hits_total Total number of cache hits\n";
    ss << "# TYPE ringcache_cache_hits_total counter\n";
    ss << "ringcache_cache_hits_total " << metrics.getHits() << "\n\n";

    ss << "# HELP ringcache_cache_misses_total Total number of cache misses\n";
    ss << "# TYPE ringcache_cache_misses_total counter\n";
    ss << "ringcache_cache_misses_total " << metrics.getMisses() << "\n\n";
    
    ss << "# HELP ringcache_keys_stored Current number of keys in storage\n";
    ss << "# TYPE ringcache_keys_stored gauge\n";
    ss << "ringcache_keys_stored " << metrics.getKeysStored() << "\n\n";

    // 2. Histogram Formatting
    // Prometheus histograms require specific labeling: {le="<boundary>"}
    // le = "less than or equal to"
    uint64_t b0 = metrics.getLatencyBucket(0); // < 1ms
    uint64_t b1 = metrics.getLatencyBucket(1); // < 5ms
    uint64_t b2 = metrics.getLatencyBucket(2); // < 10ms
    uint64_t b3 = metrics.getLatencyBucket(3); // < 50ms
    uint64_t b4 = metrics.getLatencyBucket(4); // > 50ms
    
    // Prometheus expects cumulative counts for buckets
    ss << "# HELP ringcache_request_duration_us Request latency in microseconds\n";
    ss << "# TYPE ringcache_request_duration_us histogram\n";
    ss << "ringcache_request_duration_us_bucket{le=\"1000\"} " << b0 << "\n";
    ss << "ringcache_request_duration_us_bucket{le=\"5000\"} " << (b0 + b1) << "\n";
    ss << "ringcache_request_duration_us_bucket{le=\"10000\"} " << (b0 + b1 + b2) << "\n";
    ss << "ringcache_request_duration_us_bucket{le=\"50000\"} " << (b0 + b1 + b2 + b3) << "\n";
    ss << "ringcache_request_duration_us_bucket{le=\"+Inf\"} " << (b0 + b1 + b2 + b3 + b4) << "\n";
    ss << "ringcache_request_duration_us_count " << metrics.getRequestsTotal() << "\n\n";

    return ss.str();
}

/**
 * @brief Starts a background thread that listens for HTTP GET /metrics requests
 * This is a raw TCP implementation to avoid external dependencies.
 */
inline void startMetricsServer(int port) {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        std::cerr << "Failed to create metrics socket" << std::endl;
        return;
    }

    // Allow address reuse to avoid "Address already in use" errors during restarts
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind metrics socket to port " << port << std::endl;
        close(serverFd);
        return;
    }

    if (listen(serverFd, 3) < 0) {
        std::cerr << "Failed to listen on metrics socket" << std::endl;
        close(serverFd);
        return;
    }

    std::cout << "Metrics server listening on port " << port << std::endl;

    while (true) {
        int newSocket = accept(serverFd, nullptr, nullptr);
        if (newSocket < 0) {
            continue; // Just retry if accept fails
        }

        // We don't actually care what they sent (we assume it's GET /metrics),
        // but we should read the buffer to clear the socket state.
        char buffer[1024] = {0};
        read(newSocket, buffer, 1024);

        std::string payload = generatePrometheusOutput();
        
        // Construct a valid HTTP Response
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/plain; version=0.0.4\r\n";
        response << "Content-Length: " << payload.length() << "\r\n";
        response << "Connection: close\r\n\r\n";
        response << payload;

        std::string responseStr = response.str();
        send(newSocket, responseStr.c_str(), responseStr.length(), 0);
        close(newSocket);
    }
}

#endif