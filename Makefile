CXX = clang++
#CXXFLAGS = -std=c++17 -Wall -Iinclude -g
CXXFLAGS = -std=c++17 -Wall -Iinclude -I/opt/homebrew/include -g

# 1. SHARED SOURCE FILES 
# (Everything EXCEPT src/main.cpp and src/startNode.cpp and src/startCache.cpp)
SRC_COMMON = src/client/ConsistentHashing.cpp src/server/NodeInfo.cpp src/net/TcpClient.cpp src/net/TcpServer.cpp src/server/CacheNodeServer.cpp 
OBJ_COMMON = $(patsubst src/%.cpp, build/%.o, $(SRC_COMMON))

# Default target: builds both executables when you type 'make'
all: app startNode startCache

# --- TARGET 1: Main Application ---
# Link Shared Objects + main.o
app: $(OBJ_COMMON) build/main.o
	mkdir -p bin
	$(CXX) $(OBJ_COMMON) build/main.o -o bin/app

# --- TARGET 2: Test Single Node ---
# Link Shared Objects + startNode.o
startNode: $(OBJ_COMMON) build/startNode.o
	mkdir -p bin
	$(CXX) $(OBJ_COMMON) build/startNode.o -o bin/startNode

startCache: $(OBJ_COMMON) build/startCache.o
	mkdir -p bin
	$(CXX) $(OBJ_COMMON) build/startCache.o -o bin/startCache

# --- COMPILATION RULES ---

# Generic rule for all .cpp files (handles main.cpp, startNode.cpp, and common files)
build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

check-includes:
	$(CXX) $(CXXFLAGS) -H -E src/main.cpp > /dev/null 2>&1 | grep "json.hpp"

clean:
	rm -rf build bin