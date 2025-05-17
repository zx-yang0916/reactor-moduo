#include "Reactor/Acceptor.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

namespace Reactor {

Acceptor::Acceptor(Reactor& reactor, int port) : reactor_(reactor) {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    setNonBlocking(listen_fd_);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listen_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(listen_fd_);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(listen_fd_, SOMAXCONN) < 0) {
        close(listen_fd_);
        throw std::runtime_error("Failed to listen on socket");
    }
}

Acceptor::~Acceptor() {
    if (listen_fd_ >= 0) {
        close(listen_fd_);
    }
}

void Acceptor::handleRead() {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(listen_fd_, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        if (errno != EWOULDBLOCK) {
            handleError();
        }
        return;
    }

    setNonBlocking(client_fd);
    
    if (newConnectionCallback_) {
        newConnectionCallback_(client_fd);
    } else {
        close(client_fd);
    }
}

void Acceptor::handleError() {
    std::cerr << "Error occurred in Acceptor" << std::endl;
}

void Acceptor::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("fcntl F_GETFL failed");
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("fcntl F_SETFL failed");
    }
}

} 