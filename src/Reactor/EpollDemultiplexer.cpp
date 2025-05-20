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

int EpollDemultiplexer::addEvent(EventHandler* handler, EventType event) {
    struct epoll_event ev;
    ev.data.fd = handler->getHandle();
    Handle handle = handler->getHandle();
    
    // 获取或初始化事件状态
    auto it = event_states_.find(handle);
    if (it == event_states_.end()) {
        // 新的fd，初始化事件状态
        event_states_[handle] = 0;
    }
    
    // 合并现有事件和新事件
    uint32_t& curr_events = event_states_[handle];
    curr_events |= static_cast<uint32_t>(event);
    
    // 设置epoll事件
    ev.events = curr_events | EPOLLET;  // 添加边缘触发标志

    // 尝试修改，如果失败（fd不存在）则添加
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ev);
    if (ret == -1 && errno == ENOENT) {
        ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &ev);
    }
    
    return ret;
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

int EpollDemultiplexer::removeEvent(EventHandler* handler, EventType event) {
    struct epoll_event ev;
    Handle handle = handler->getHandle();
    
    // 检查fd是否在事件状态表中
    auto it = event_states_.find(handle);
    if (it == event_states_.end()) {
        return -1;  // fd不存在
    }
    
    // 从当前事件中移除指定事件
    uint32_t& curr_events = it->second;
    curr_events &= ~static_cast<uint32_t>(event);
    
    if (curr_events == 0) {
        // 如果没有剩余事件，从epoll中删除fd
        event_states_.erase(it);
        return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handle, nullptr);
    } else {
        // 更新epoll事件
        ev.data.fd = handle;
        ev.events = curr_events | EPOLLET;
        return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ev);
    }
}

int EpollDemultiplexer::modifyEvent(EventHandler* handler, EventType event) {
    struct epoll_event ev;
    Handle handle = handler->getHandle();
    
    // 检查fd是否在事件状态表中
    auto it = event_states_.find(handle);
    if (it == event_states_.end()) {
        // fd不存在，直接添加新事件
        event_states_[handle] = static_cast<uint32_t>(event);
        ev.data.fd = handle;
        ev.events = event_states_[handle] | EPOLLET;
        return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &ev);
    }
    
    // 更新事件状态
    it->second = static_cast<uint32_t>(event);
    
    // 设置epoll事件
    ev.data.fd = handle;
    ev.events = it->second | EPOLLET;
    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ev);
}

}