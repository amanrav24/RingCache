#include <net/TcpClient.hpp>


tcpClient::tcpClient(std::string hostString, int port, std::string ipAddr):  host(hostString), port(port), ipAddr(ipAddr) {
}


bool tcpClient::connectToServer() {
    clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1) {
        std::cout << "Socket creation failed" << std::endl;
        clientFd = -1;
        return false;
    }

    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // Force IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    std::string portStr = std::to_string(port);
    std::cout << ipAddr.c_str() << std::endl;
    int status = getaddrinfo(ipAddr.c_str(), portStr.c_str(), &hints, &res);
    if (status != 0) {
        std::cout << "Address resolution failed: " << gai_strerror(status) << std::endl;
        close(clientFd);
        clientFd = -1;
        return false;
    }


    if (connect(clientFd, res->ai_addr, res->ai_addrlen) < 0) {
        std::cout << "connect failed" << std::endl;
        freeaddrinfo(res); // Important: free memory before returning
        close(clientFd);
        clientFd = -1;
        return false;
    }

    freeaddrinfo(res);
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