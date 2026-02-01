#pragma once

//server needs to listen for incoming requets
//send out 

#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>


class tcpServer {
public:
    using clientHandler = std::function<void(int clientFd)>;

    explicit tcpServer(int port);

    tcpServer(const tcpServer&) = delete;
    tcpServer& operator=(const tcpServer&) = delete;

    void start(const clientHandler& handler);

    void sendResponse(const std::string& data, int clientFd);
    std::string readLine(int serverFd);
private:
    int serverFd;
    int port;

    void startSocket();
};