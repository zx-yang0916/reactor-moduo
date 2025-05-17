#pragma once

#include "EventHandler.h"
#include <functional>

namespace Reactor {

class Reactor;

class Acceptor : public EventHandler {
public:
    using NewConnectionCallback = std::function<void(Handle)>;

    explicit Acceptor(Reactor& reactor, int port);
    ~Acceptor() override;

    Handle getHandle() const override { return listen_fd_; }
    void handleRead() override;
    void handleWrite() override {}
    void handleError() override;

    void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }

private:
    Handle listen_fd_;
    Reactor& reactor_;
    NewConnectionCallback newConnectionCallback_;

    void setNonBlocking(int fd);
};

} 