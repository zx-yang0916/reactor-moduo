// #include "Reactor/SockHandle.h"
// #include <unistd.h>
// #include <iostream>
// #include <cstring>

// namespace Reactor {

// SockHandle::SockHandle(Handle fd) : sock_fd_(fd) {
//     buffer_.resize(BUFFER_SIZE);
// }

// SockHandle::~SockHandle() {
//     if (sock_fd_ >= 0) {
//         close(sock_fd_);
//     }
// }

// void SockHandle::handleRead() {
//     ssize_t bytes_read = read(sock_fd_, buffer_.data(), buffer_.size());
//     if (bytes_read > 0) {
//         // 简单回显(Echo)功能
//         write(sock_fd_, buffer_.data(), bytes_read);
//     } else if (bytes_read == 0) {
//         // 客户端关闭连接
//         handleError();
//     } else {
//         if (errno != EWOULDBLOCK) {
//             handleError();
//         }
//     }
// }

// void SockHandle::handleError() {
//     if (sock_fd_ >= 0) {
//         close(sock_fd_);
//         sock_fd_ = -1;
//     }
// }

// } // namespace Reactor