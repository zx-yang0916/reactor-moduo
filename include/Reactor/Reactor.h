#pragma once

#include "EventHandler.h"
#include <memory>
#include "ReactorImplementation.h" 

namespace Reactor {

class Reactor {
public:
    Reactor();
    ~Reactor();

    int registerHandler(EventHandler* handler, EventType event);
    void removeHandler(EventHandler* handler);
    void modifyHandler(EventHandler* handler, EventType event);
    void eventLoop(int timeout = 10); // 10ms

private:
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    std::unique_ptr<ReactorImplementation> impl_;
};

} // namespace Reactor