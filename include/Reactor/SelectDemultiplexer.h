#pragma once

#include "EventDemultiplexer.h"
#include "EventHandler.h"
#include <vector>


namespace Reactor {

class SelectDemultiplexer : public EventDemultiplexer {
public:
    SelectDemultiplexer() = default;
    ~SelectDemultiplexer() override;

    int registerHandler(EventHandler* handler, EventType event) override;
    int waitEvent(std::unordered_map<Handle, EventHandler*>& handlers, int timeout = 0) override;
    int removeHandler(Handle handle) override;

private:
    using fd_set = std::vector<Handle>;
    fd_set read_set_;
    fd_set write_set_;
    fd_set err_set_;
};

}