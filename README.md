# RingCache

RingCache is a **Redis-inspired, distributed in-memory key–value cache** written in modern C++. It is designed as a **systems-focused learning project** that explores how real-world caches work under the hood, including networking, consistent hashing, concurrency, eviction policies, and metrics.

This project prioritizes **clarity of architecture and correctness** with performance in mind.

---

## ✨ Features

* **In-memory key–value storage**
* **Ring-based cache implementation** with fixed capacity
* **Consistent hashing** for scalable key distribution across nodes
* **LRU-style eviction** (via ring buffer semantics)
* **Multi-node cache architecture**
* **TCP-based client–server communication**
* **Thread-safe design** (mutex-protected critical sections)
* **Header-only core data structures** for easy reuse
* **Atomic Metrics System** for efficient logging

---

## 🧠 System Architecture

At a high level, RingCache is composed of three layers:

```
Client
  ↓ (TCP)
CacheNodeServer
  ↓
CacheNode
  ↓
RingCache (local storage)
```

### Components

#### 1. `RingCache`

* A fixed-size cache implemented as a **circular buffer**
* Automatically evicts old entries when capacity is exceeded
* Provides `put` and `get` semantics

#### 2. `CacheNode`

* Owns a local `RingCache`
* Responsible for executing cache operations
* Stateless with respect to networking

#### 3. `ConsistentHashing`

* Maps keys to cache nodes using a hash ring
* Minimizes key redistribution when nodes join or leave

#### 4. `CacheNodeServer`

* TCP server responsible for:

  * Accepting client connections
  * Parsing requests
  * Dispatching commands to the appropriate `CacheNode`
* Clean separation between **networking** and **storage logic**

---

## 📁 Project Structure

```
ring-cache/

├── Makefile
├── include
│   ├── client
│   │   ├── ConsistentHashing.hpp
│   │   └── RingCache.hpp
│   ├── conf
│   │   └── nodeConfig.hpp
│   ├── metrics
│   │   ├── MetricsRegistry.hpp
│   │   └── MetricsServer.hpp
│   ├── net
│   │   ├── TcpClient.hpp
│   │   └── TcpServer.hpp
│   └── server
│       ├── CacheNodeServer.hpp
│       └── NodeInfo.hpp
└── src
    ├── client
    │   └── ConsistentHashing.cpp
    ├── main.cpp
    ├── net
    │   ├── TcpClient.cpp
    │   └── TcpServer.cpp
    ├── server
    │   ├── CacheNodeServer.cpp
    │   └── NodeInfo.cpp
    ├── startCache.cpp
    └── startNode.cpp
├── cacheDocker
│   └── Dockerfile
├── nodeDocker
│   └── Dockerfile
├── docker-compose.yaml
├── prometheus.yml
```

---

## 🧩 Key Design Decisions

### Why a Ring Cache?

* Predictable memory usage
* Simple eviction semantics
* Models how bounded caches work at a low level

### Why Consistent Hashing?

* Avoids full cache reshuffling on node changes
* Used by real systems like **Redis Cluster** and **DynamoDB**

### Why TCP instead of HTTP?

* Lower overhead
* Forces explicit handling of:

  * Framing
  * Serialization
  * Partial reads/writes

---

## 🧵 Concurrency Model

* Each client connection is handled independently
* Shared data structures are protected via `std::mutex`
* Designed to be extended with:

  * Thread pools
  * Lock-free structures

---

## 🚀 Getting Started

### Build

```bash
mkdir build
cd build
cmake ..
make
```

### Run a Node

```bash
./ring_cache_node --port 6379
```

### Example Client Request

```
SET mykey hello
GET mykey
```
---

## 📈 Future Improvements

* Replication and fault tolerance
* Write-ahead logging (durability)
* TTL / expiration support
* Async I/O (`epoll` / `io_uring`)
* Binary protocol instead of text-based commands
* Metrics and observability

---

## 🎯 Learning Goals

This project demonstrates proficiency in:

* Modern C++ (RAII, smart pointers, templates)
* Systems design and tradeoffs
* Networking fundamentals
* Distributed systems concepts
* Testing and visualizing with Docker, Prometheus, and Grafana
* Use c++ std::memory_order_relaxed to understand metrics

---

## 📜 License

MIT License

---

## 👤 Author

**Aman Ravishankar**
CS + Statistics @ UIUC

---

If you’re interested in discussing design tradeoffs or extending this project, feel free to reach out!
