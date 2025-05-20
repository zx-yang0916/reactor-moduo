#pragma once

#include "EventHandler.h"
#include <functional>

namespace Reactor {

class Reactor;

class Acceptor : public EventHandler {
public:
    using NewConnectionCallback = std::function<void(Handle)>;

    explicit Acceptor(Reactor& reactor, int port = 8080);   // 负责监听
    ~Acceptor() override;

    Handle getHandle() const override { return listen_fd_; }
    void handleRead() override;  // 处理读事件，即连接事件
    void handleWrite() override {}   // 不需要写
    void handleError() override;    // 处理错误事件

    // 新连接来到如何处理，分发到subReactor，subReactor由MainReactor管理
    void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }

private:
    Handle listen_fd_;
    Reactor& reactor_;
    NewConnectionCallback newConnectionCallback_;

    void setNonBlocking(int fd);
};

} 