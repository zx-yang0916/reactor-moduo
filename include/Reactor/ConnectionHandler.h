#pragma once

#include "EventHandler.h"
#include <vector>
#include <memory>
#include <functional>

namespace Reactor {

class ConnectionHandler : public EventHandler {
public:
    using MessageCallback = std::function<void(const char*, size_t)>;
    using CloseCallback = std::function<void(Handle)>;

    explicit ConnectionHandler(Handle fd);
    ~ConnectionHandler() override;

    Handle getHandle() const override { return sock_fd_; }
    void handleRead() override;
    void handleWrite() override;
    void handleError() override;

    void send(const char* data, size_t len);
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }   // 设置消息回调
    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }         // 设置关闭回调

private:
    Handle sock_fd_;
    static const size_t BUFFER_SIZE = 1024;
    std::vector<char> readBuffer_;
    std::vector<char> writeBuffer_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;

    void setNonBlocking(int fd);
};

using ConnectionHandlerPtr = std::shared_ptr<ConnectionHandler>;

} 