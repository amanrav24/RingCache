#include <net/TcpClient.hpp>


tcpClient::tcpClient(std::string hostString, int port):  host(hostString), port(port) {
}


bool tcpClient::connectToServer() {
    clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1) {
        clientFd = -1;
        return false;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    //local-host for testing
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
        close(clientFd);
        clientFd = -1;
        return false;
    }

    if (connect(clientFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(clientFd);
        clientFd = -1;
        return false;
    }

    return true;
}

std::string tcpClient::receiveLine() {
    if (clientFd == -1) {
        return "";
    }

    std::string result;
    char buffer[1024];

    while (true) {
        size_t loc = result.find('\n');
        if (loc != std::string::npos) {
            return result.substr(0, loc);
        }

        ssize_t bytesRecieved = recv(clientFd, buffer, sizeof(buffer), 0);

        if (bytesRecieved < 0) {
            return "";
        }

        if (bytesRecieved == 0) {
            return result;
        }

        result.append(buffer, bytesRecieved);
    }

    return result;
}


bool tcpClient::sendAll(const std::string& data) {
    if (clientFd == -1) {
        return false;
    }

    size_t totalSent = 0;
    size_t bytesLeft = data.length();
    const char *ptr = data.c_str();


    while (totalSent < data.length()) {
        ssize_t bytesSent = send(clientFd, ptr + totalSent, bytesLeft, 0);

        if (bytesSent < 0) {
            return false;
        }

        totalSent += bytesSent;
        bytesLeft -= bytesSent;
    }

    return true;
}

tcpClient::~tcpClient() {
    if (clientFd != -1) {
        close(clientFd);
    }
}       