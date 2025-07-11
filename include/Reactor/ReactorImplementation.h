#pragma once

#include "EventHandler.h"
#include "EventDemultiplexer.h"
#include <memory>
#include <unordered_map>


namespace Reactor {


class ReactorImplementation {
public:
    ReactorImplementation();
    ~ReactorImplementation();

    int registerHandler(EventHandler* handler, EventType event);
    void removeHandler(EventHandler* handler);
    void modifyHandler(EventHandler* handler, EventType event);
    void eventLoop(int timeout = 10); // 10ms

private:
    std::unique_ptr<EventDemultiplexer> demultiplexer_;            // 事件多路复用器
    std::unordered_map<Handle, EventHandler*> handlers_;           // 事件处理器
};

} // namespace