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

// void ConnectionHandler::handleRead() {
//     ssize_t bytes_read = read(sock_fd_, readBuffer_.data(), readBuffer_.size());
//     if (bytes_read > 0) {
//         if (messageCallback_) {
//             messageCallback_(readBuffer_.data(), bytes_read);
//         }
//         // 回显功能
//         send(readBuffer_.data(), bytes_read);
//     } else if (bytes_read == 0) {
//         // 客户端关闭连接
//         handleError();
//     } else {
//         if (errno != EWOULDBLOCK) {
//             handleError();
//         }
//     }
// }
void ConnectionHandler::handleRead() {
    while (true) {
        ssize_t bytes_read = read(sock_fd_, readBuffer_.data(), readBuffer_.size());
        if (bytes_read > 0) {
            if (messageCallback_) {
                messageCallback_(readBuffer_.data(), bytes_read);
            }
            send(readBuffer_.data(), bytes_read);
        } else if (bytes_read == 0) {
            handleError();
            return;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;  // 非阻塞模式下，没有数据可读，退出循环
            }
            handleError();
            return;
        }
    }
}

void ConnectionHandler::handleWrite() {
    while (!writeBuffer_.empty()) {
        ssize_t bytes_written = write(sock_fd_, writeBuffer_.data(), writeBuffer_.size());
        if (bytes_written > 0) {
            writeBuffer_.erase(writeBuffer_.begin(), writeBuffer_.begin() + bytes_written);
        } else if (bytes_written < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // 写缓冲区已满，等待下一次写事件
                break;
            }
            // 如果是其他错误，则调用handleError
            handleError();
            return;
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
    if (sock_fd_ < 0) {
        return;  // 连接已关闭
    }

    // 如果写缓冲区为空，尝试直接写入
    if (writeBuffer_.empty()) {
        ssize_t bytes_written = write(sock_fd_, data, len);
        if (bytes_written >= 0) {
            // 如果没有写完全部数据，将剩余数据放入缓冲区
            if (static_cast<size_t>(bytes_written) < len) {
                size_t remaining = len - bytes_written;
                size_t old_size = writeBuffer_.size();
                writeBuffer_.resize(old_size + remaining);
                std::memcpy(writeBuffer_.data() + old_size, data + bytes_written, remaining);
            }
        } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // 写缓冲区已满，将所有数据放入写缓冲区
            size_t old_size = writeBuffer_.size();
            writeBuffer_.resize(old_size + len);
            std::memcpy(writeBuffer_.data() + old_size, data, len);
        } else {
            // 发生错误
            handleError();
            return;
        }
    } else {
        // 写缓冲区不为空，直接将数据追加到缓冲区
        size_t old_size = writeBuffer_.size();
        writeBuffer_.resize(old_size + len);
        std::memcpy(writeBuffer_.data() + old_size, data, len);
    }
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