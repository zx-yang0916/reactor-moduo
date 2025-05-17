#include "Reactor/SelectDemultiplexer.h"
#include <algorithm>
#include <sys/select.h>
#include <iostream>
#include <cstring>  // for strerror
#include <cerrno>   // for errno

namespace Reactor {

SelectDemultiplexer::SelectDemultiplexer() : max_fd_(-1) {
    FD_ZERO(&read_fds_);
    FD_ZERO(&write_fds_);
    FD_ZERO(&error_fds_);
    FD_ZERO(&read_fds_backup_);
    FD_ZERO(&write_fds_backup_);
    FD_ZERO(&error_fds_backup_);
}

int SelectDemultiplexer::registerHandler(EventHandler* handler, EventType event) {
    Handle handle = handler->getHandle();
    
    if (event & ReadEvent) {
        FD_SET(handle, &read_fds_backup_);
    }
    if (event & WriteEvent) {
        FD_SET(handle, &write_fds_backup_);
    }
    FD_SET(handle, &error_fds_backup_);

    max_fd_ = std::max(max_fd_, handle);
    return 0;
}

int SelectDemultiplexer::removeHandler(Handle handle) {
    FD_CLR(handle, &read_fds_backup_);
    FD_CLR(handle, &write_fds_backup_);
    FD_CLR(handle, &error_fds_backup_);

    // 更新max_fd_
    if (handle == max_fd_) {
        max_fd_ = -1;
        for (Handle fd = 0; fd <= handle; ++fd) {
            if (FD_ISSET(fd, &read_fds_backup_) || 
                FD_ISSET(fd, &write_fds_backup_) || 
                FD_ISSET(fd, &error_fds_backup_)) {
                max_fd_ = fd;
            }
        }
    }
    return 0;
}

int SelectDemultiplexer::waitEvent(std::unordered_map<Handle, EventHandler*>& handlers, int timeout) {
    // 复制fd_set，因为select会修改它们
    read_fds_ = read_fds_backup_;
    write_fds_ = write_fds_backup_;
    error_fds_ = error_fds_backup_;

    if (max_fd_ == -1) {
        return 0;
    }

    timeval tv{};
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    int num_events = select(max_fd_ + 1, &read_fds_, &write_fds_, &error_fds_, &tv);
    
    if (num_events < 0) {
        if (errno == EINTR) {
            return 0;
        }
        std::cerr << "select error: " << strerror(errno) << std::endl;
        return -1;
    }

    if (num_events == 0) {
        return 0;
    }

    for (Handle fd = 0; fd <= max_fd_; ++fd) {
        if (FD_ISSET(fd, &error_fds_)) {
            auto it = handlers.find(fd);
            if (it != handlers.end()) {
                it->second->handleError();
            }
            continue;
        }
        
        if (FD_ISSET(fd, &read_fds_)) {
            auto it = handlers.find(fd);
            if (it != handlers.end()) {
                it->second->handleRead();
            }
        }
        
        if (FD_ISSET(fd, &write_fds_)) {
            auto it = handlers.find(fd);
            if (it != handlers.end()) {
                it->second->handleWrite();
            }
        }
    }

    return num_events;
}

} 