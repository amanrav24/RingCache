# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build all targets (app, startNode, startCache)
make

# Build individual targets
make startNode    # Cache node server binary
make startCache   # Load test client binary
make app          # Simple single-node test harness

# Clean build artifacts
make clean
```

**Compiler:** `clang++` with `-std=c++17`. On macOS, Homebrew includes are pulled from `/opt/homebrew/include` (for `nlohmann/json.hpp`). On Linux/Docker, `nlohmann-json3-dev` is installed via apt.

## Running the System

**Local single-node test** (requires node running separately on port 8080):
```bash
./bin/app
```

**Run a cache node** (required env vars):
```bash
NODE_ID=node-1 NODE_IP=127.0.0.1 PORT=8080 ./bin/startNode
```

**Run the load test client** (requires nodes already running):
```bash
NODE_LIST=node-1:8080,node-2:8081 ./bin/startCache
```

**Multi-node cluster via Docker:**
```bash
docker-compose up --build
```
This spins up 5 cache nodes + 1 load-test client + Prometheus (port 9090) + Grafana (port 3000, login: admin/admin).

## Architecture

The system has two separate process roles:

**Cache Node** (`startNode`): A server process that stores key-value data. Each node runs independently on a configured port. Reads env vars `NODE_ID`, `NODE_IP`, `PORT` at startup.

**Cache Client** (`startCache` / `app`): A client process that holds a `ringCache<K,V>` instance. The client is responsible for routing — it uses `ConsistentHashingRing` to determine which node owns a given key, then opens a TCP connection to that node.

### Request flow
```
ringCache::cacheSet/cacheGet/cacheDelete
  → ConsistentHashingRing::getNode(key)  [hash ring lookup]
  → tcpClient::connectToServer()          [raw TCP connect]
  → send JSON: {"op":"set","key":...,"value":...}
  → CacheNodeServer::clientHandler()      [node receives]
  → NodeInfo::nodeSet/nodeGet/nodeDelete  [local storage]
  → JSON response: {"status":"success","value":...}
```

### Key components

| Component | Files | Role |
|-----------|-------|------|
| `ringCache<K,V>` | `include/client/RingCache.hpp` | Client-side template class; owns hash ring + TCP clients per node |
| `ConsistentHashingRing` | `include/client/ConsistentHashing.hpp`, `src/client/ConsistentHashing.cpp` | Hash ring mapping keys to node IDs |
| `CacheNodeServer` | `include/server/CacheNodeServer.hpp`, `src/server/CacheNodeServer.cpp` | TCP server; parses JSON commands, dispatches to NodeInfo |
| `NodeInfo` | `include/server/NodeInfo.hpp`, `src/server/NodeInfo.cpp` | Per-node key-value store with TTL; uses a priority queue for background expiry cleanup |
| `tcpClient` / `tcpServer` | `include/net/`, `src/net/` | Thin POSIX socket wrappers |
| `MetricsRegistry` | `include/metrics/MetricsRegistry.hpp` | Lock-free singleton (atomics) tracking requests, hits, misses, evictions, latency histogram |
| `MetricsServer` | `include/metrics/MetricsServer.hpp` | Raw HTTP server (port 9090) serving Prometheus exposition format at `/metrics` |
| `nodeConfig` | `include/conf/nodeConfig.hpp` | Plain struct: `{nodeId, ipAddress, port}` |

### Wire protocol
Newline-delimited JSON over TCP. Request: `{"op":"set"|"get"|"del","key":"...","value":"..."}`. Response: `{"status":"success"|"failed","value":"..."}`.

### TTL / expiry
All keys set with a 5-second TTL (hardcoded in `CacheNodeServer::handleCommand`). `NodeInfo` uses both lazy expiration (checked on `get`) and a background thread that wakes every 5 seconds to drain the TTL priority queue.

### Metrics
Each cache node exposes Prometheus metrics on port 9090 via a background thread launched in `startNode.cpp`. `MetricsRegistry` is a Meyer's singleton using `std::atomic` — no mutex on the hot path.

## Include structure
Headers live in `include/` and are namespaced by subdirectory (`client/`, `server/`, `net/`, `metrics/`, `conf/`). The Makefile passes `-Iinclude` so includes are written as `#include <client/RingCache.hpp>`, not relative paths.
