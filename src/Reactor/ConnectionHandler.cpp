#include "Reactor/ConnectionHandler.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>

namespace Reactor {

ConnectionHandler::ConnectionHandler(Handle fd) : sock_fd_(fd) {
    readBuffer_.resize(BUFFER_SIZE);
    setNonBlocking(fd);
}

ConnectionHandler::~ConnectionHandler() {
    if (sock_fd_ >= 0) {
        close(sock_fd_);
    }
}

void ConnectionHandler::handleRead() {
    ssize_t bytes_read = read(sock_fd_, readBuffer_.data(), readBuffer_.size());
    if (bytes_read > 0) {
        if (messageCallback_) {
            messageCallback_(readBuffer_.data(), bytes_read);
        }
        // Echo functionality
        send(readBuffer_.data(), bytes_read);
    } else if (bytes_read == 0) {
        // Client closed connection
        handleError();
    } else {
        if (errno != EWOULDBLOCK) {
            handleError();
        }
    }
}

void ConnectionHandler::handleWrite() {
    if (!writeBuffer_.empty()) {
        ssize_t bytes_written = write(sock_fd_, writeBuffer_.data(), writeBuffer_.size());
        if (bytes_written > 0) {
            writeBuffer_.erase(writeBuffer_.begin(), writeBuffer_.begin() + bytes_written);
        } else if (bytes_written < 0 && errno != EWOULDBLOCK) {
            handleError();
        }
    }
}

void ConnectionHandler::handleError() {
    if (closeCallback_) {
        closeCallback_(sock_fd_);
    }
    if (sock_fd_ >= 0) {
        close(sock_fd_);
        sock_fd_ = -1;
    }
}

void ConnectionHandler::send(const char* data, size_t len) {
    size_t old_size = writeBuffer_.size();
    writeBuffer_.resize(old_size + len);
    std::memcpy(writeBuffer_.data() + old_size, data, len);
}

void ConnectionHandler::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("fcntl F_GETFL failed");
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("fcntl F_SETFL failed");
    }
}

} 