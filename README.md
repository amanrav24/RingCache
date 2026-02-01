# RingCache

RingCache is a **Redis-inspired, distributed in-memory key–value cache** written in modern C++. It is designed as a **systems-focused learning project** that explores how real-world caches work under the hood, including networking, consistent hashing, concurrency, and eviction policies.

This project prioritizes **clarity of architecture and correctness** over raw performance, making it ideal for learning and experimentation.

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
* Supports virtual nodes for better load balancing

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
├── include/
│   ├── RingCache.hpp
│   ├── ConsistentHashing.hpp
│   ├── NodeInfo.hpp
│   └── CacheNode.hpp
├── src/
│   ├── CacheNodeServer.cpp
│   ├── main.cpp
│   └── utils.cpp
├── tests/
│   └── ring_cache_tests.cpp
├── CMakeLists.txt
└── README.md
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

## 🧪 Testing

Unit tests validate:

* Ring buffer eviction behavior
* Correct key placement via consistent hashing
* Edge cases (empty cache, overwrite, wraparound)

Run tests:

```bash
ctest
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

---

## 📜 License

MIT License

---

## 👤 Author

**Aman Ravishankar**
CS + Statistics @ UIUC

---

If you’re interested in discussing design tradeoffs or extending this project, feel free to reach out!
