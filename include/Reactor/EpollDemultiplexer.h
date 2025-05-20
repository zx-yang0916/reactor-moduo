#pragma once

#include "EventDemultiplexer.h"
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>

namespace Reactor {

class EpollDemultiplexer : public EventDemultiplexer {
public:
    EpollDemultiplexer();
    ~EpollDemultiplexer() override;

    int addEvent(EventHandler* handler, EventType event);
    int removeEvent(EventHandler* handler, EventType event);
    int modifyEvent(EventHandler* handler, EventType event);

    int registerHandler(EventHandler* handler, EventType event) override;
    int waitEvent(std::unordered_map<Handle, EventHandler*>& handlers, int timeout = 0) override;
    int removeHandler(Handle handle) override;

private:
    static const int MAX_EVENTS = 1024;
    int epoll_fd_;
    std::vector<struct epoll_event> events_;
    std::unordered_map<Handle, uint32_t> event_states_;  // 跟踪每个fd的事件状态
};

}