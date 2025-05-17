#pragma once
#include <sys/epoll.h>

namespace Reactor {

using Handle = int;

enum EventType {
    ReadEvent = EPOLLIN,
    WriteEvent = EPOLLOUT,
    ErrorEvent = EPOLLERR
};

class EventHandler {
public:
    virtual ~EventHandler() = default;

    virtual Handle getHandle() const = 0;
    virtual void handleRead() = 0;
    virtual void handleWrite() = 0;
    virtual void handleError() = 0;
};


} // namespace Ractor
