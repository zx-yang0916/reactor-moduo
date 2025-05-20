#pragma once

#include "Reactor.h"
#include "ConnectionHandler.h"
#include <thread>
#include <memory>
#include <atomic>
#include <unordered_map>

namespace Reactor {

class SubReactor {
public:
    SubReactor();
    ~SubReactor();

    void start();
    void stop();
    void addConnection(Handle fd);   // 添加客户端连接

private:
    void threadFunc();

    Reactor reactor_;
    std::unique_ptr<std::thread> thread_;
    std::atomic<bool> running_{false};
    std::unordered_map<Handle, ConnectionHandlerPtr> connections_;  // 主要管理连接
};

using SubReactorPtr = std::shared_ptr<SubReactor>;

} 