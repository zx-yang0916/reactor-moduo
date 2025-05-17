// #pragma once

// #include "EventHandler.h"

// namespace Reactor {

// class Reactor;

// class ListenHandle : public EventHandler {
// public:
//     explicit ListenHandle(Reactor& reactor, int port);
//     ~ListenHandle() override;

//     Handle getHandle() const override { return listen_fd_; }
//     void handleRead() override;
//     void handleWrite() override {}
//     void handleError() override;
// private:
//     Handle listen_fd_;
//     Reactor& reactor_;

//     void setNonBlocking(int fd);
// };

// }