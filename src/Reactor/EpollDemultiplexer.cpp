#include "Reactor/EpollDemultiplexer.h"
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>


namespace Reactor {
EpollDemultiplexer::EpollDemultiplexer() {
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }
    events_.resize(MAX_EVENTS);
}

EpollDemultiplexer::~EpollDemultiplexer() {
    close(epoll_fd_);
}

int EpollDemultiplexer::registerHandler(EventHandler* handler, EventType event) {
    struct epoll_event ev;
    ev.data.fd = handler->getHandle();
    ev.events = 0;

    if (event & EventType::ReadEvent) ev.events |= EPOLLIN;
    if (event & EventType::WriteEvent) ev.events |= EPOLLOUT;
    ev.events |= EPOLLET;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->getHandle(), &ev);
}

int EpollDemultiplexer::waitEvent(std::unordered_map<Handle, EventHandler*>& handlers, int timeout) {
    int num_events = epoll_wait(epoll_fd_, events_.data(), MAX_EVENTS, timeout);
    if (num_events == -1) {
        return -1;
    }

    for (int i = 0; i < num_events; ++i) {
        Handle handle = events_[i].data.fd;
        auto it = handlers.find(handle);
        if (it == handlers.end()) continue;

        EventHandler* handler = it->second;
        if (events_[i].events & EPOLLIN) {
            handler->handleRead();
        }
        if (events_[i].events & EPOLLOUT) {
            handler->handleWrite();
        }
        if (events_[i].events & (EPOLLERR | EPOLLHUP)) {
            handler->handleError();
        }
    }
    return num_events;
}

int EpollDemultiplexer::removeHandler(Handle handle) {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handle, nullptr);
}


}