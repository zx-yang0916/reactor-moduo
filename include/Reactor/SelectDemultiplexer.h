#pragma once

#include "EventDemultiplexer.h"
#include <sys/select.h>
#include <vector>
#include <unordered_map>

namespace Reactor {

class SelectDemultiplexer : public EventDemultiplexer {
public:
    SelectDemultiplexer();
    ~SelectDemultiplexer() override = default;

    int registerHandler(EventHandler* handler, EventType event) override;
    int removeHandler(Handle handle) override;
    int waitEvent(std::unordered_map<Handle, EventHandler*>& handlers, int timeout) override;

private:
    fd_set read_fds_;
    fd_set write_fds_;
    fd_set error_fds_;
    fd_set read_fds_backup_;
    fd_set write_fds_backup_;
    fd_set error_fds_backup_;
    Handle max_fd_;
};

}