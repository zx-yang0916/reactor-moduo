#pragma once

#include "EventHandler.h"
#include <memory>
#include "ReactorImplementation.h" 

namespace Reactor {

class Reactor {
public:
    static Reactor& getInstance();

    int registerHandler(EventHandler* handler, EventType event);
    void removeHandler(EventHandler* handler);
    void modifyHandler(EventHandler* handler, EventType event);
    void eventLoop(int timeout = 10); // 10ms

private:
    Reactor();
    ~Reactor() = default;

    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    // class ReactorImplementation;
    std::unique_ptr<ReactorImplementation> impl_;
};

} // namespace Reactor