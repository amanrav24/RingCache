#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>



class tcpClient {
public:
    tcpClient(std::string hostString, int port, std::string ipAddr);
    ~tcpClient();

    tcpClient(const tcpClient&) = delete;
    tcpClient& operator=(const tcpClient&) = delete;

    bool connectToServer();

    bool sendAll(const std::string& data);

    std::string receiveLine();
private:
    std::string host;
    int port;
    std::string ipAddr;

    int clientFd;
};