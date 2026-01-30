CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Iinclude -I/opt/homebrew/include

SRC = src/client/ConsistentHashing.cpp src/server/NodeInfo.cpp src/main.cpp src/client/RingCache.cpp src/net/TcpClient.cpp src/net/TcpServer.cpp
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

app: $(OBJ)
	mkdir -p bin
	$(CXX) $(OBJ) -o bin/app


build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

check-includes:
	$(CXX) $(CXXFLAGS) -H -E src/main.cpp > /dev/null 2>&1 | grep "json.hpp"

clean:
	rm -rf build bin