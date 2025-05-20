#pragma once
#include <sys/epoll.h>

namespace Reactor {

using Handle = int;

enum EventType {
    NoneEvent = 0,
    ReadEvent = EPOLLIN,
    WriteEvent = EPOLLOUT,
    ErrorEvent = EPOLLERR,
    // 预定义一些常用的组合
    ReadWrite = ReadEvent | WriteEvent
};

// 支持位运算操作
inline EventType operator|(EventType a, EventType b) {
    return static_cast<EventType>(static_cast<int>(a) | static_cast<int>(b));
}

inline EventType operator&(EventType a, EventType b) {
    return static_cast<EventType>(static_cast<int>(a) & static_cast<int>(b));
}

inline EventType operator~(EventType a) {
    return static_cast<EventType>(~static_cast<int>(a));
}

inline EventType& operator|=(EventType& a, EventType b) {
    return a = a | b;
}

inline EventType& operator&=(EventType& a, EventType b) {
    return a = a & b;
}

class EventHandler {
public:
    virtual ~EventHandler() = default;

    virtual Handle getHandle() const = 0;
    virtual void handleRead() = 0;
    virtual void handleWrite() = 0;
    virtual void handleError() = 0;
};

} // namespace Reactor
