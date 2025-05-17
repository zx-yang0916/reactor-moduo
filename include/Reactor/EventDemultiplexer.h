#pragma once
#include "EventHandler.h"
#include <unordered_map>

namespace Reactor {

class EventDemultiplexer {
public:
    virtual ~EventDemultiplexer() = default;

    virtual int registerHandler(EventHandler* handler, EventType event) = 0;
    virtual int waitEvent(std::unordered_map<Handle, EventHandler*>& handlers, int timeout = 0) = 0;
    virtual int removeHandler(Handle handle) = 0;
};
}