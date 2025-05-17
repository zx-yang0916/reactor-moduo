#pragma once

#include "EventHandler.h"
#include "Reactor.h"
#include <vector>

namespace Reactor {

class SockHandle : public EventHandler {
public:
    explicit SockHandle(Handle fd);
    ~SockHandle() override;

    Handle getHandle() const override { return sock_fd_; }
    void handleRead() override;
    void handleWrite() override {}
    void handleError() override;
private:
    Handle sock_fd_;
    
    static const size_t BUFFER_SIZE = 1024;
    std::vector<char> buffer_;
};

}