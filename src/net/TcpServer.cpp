#include <net/TcpServer.hpp>


void tcpServer::start(const clientHandler& handler) {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    while (1) {
        int clientFd = accept(serverFd, (struct sockaddr*)& clientAddr, &clientLen); 

        if (clientFd < 0) {
            //Issue connecting client
            continue;
        }

        handler(clientFd);
        close(clientFd);
    }
}

tcpServer::tcpServer(int inPort) : port(inPort), serverFd(-1) {
    try {
        startSocket();
    } catch (const std::exception& e) {
        if (serverFd != -1) {
            serverFd = -1;
        }
        throw;
    }
}

void tcpServer::startSocket() {
    //socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        throw std::runtime_error("Server: Socket Creation Failed");
    }

    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to set socket options");
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port);

    //bind
    if (bind(serverFd,(struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Failed to bind socket to address");
    }

    //listen
    if (listen(serverFd, 10) < 0) {
        throw std::runtime_error("Failed to listen");
    }
}


void tcpServer::sendResponse(const std::string& data) {
    if (serverFd == -1) {
        return;
    }

    size_t totalSent = 0;
    size_t bytesLeft = data.length();
    const char *ptr = data.c_str();

    while (totalSent < data.length()) {
        ssize_t bytesSent = send(serverFd, ptr + totalSent, bytesLeft, 0);

        if (bytesSent < 0) {
            return;
        }

        totalSent += bytesSent;
        bytesLeft -= bytesSent;
    }

    return;
}

std::string tcpServer::readLine(int serverFd) {
    if (serverFd == -1) {
        return "";
    }

    std::string result;
    char buffer[1024];

    while (true) {
        size_t loc = result.find('\n');
        if (loc != std::string::npos) {
            return result.substr(0, loc + 1);
        }

        ssize_t bytesRecieved = recv(serverFd, buffer, sizeof(buffer), 0);

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