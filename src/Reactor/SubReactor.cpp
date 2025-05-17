#include "Reactor/SubReactor.h"
#include <iostream>

namespace Reactor {

SubReactor::SubReactor() = default;

SubReactor::~SubReactor() {
    stop();
}

void SubReactor::start() {
    if (!running_) {
        running_ = true;
        thread_ = std::make_unique<std::thread>(&SubReactor::threadFunc, this);
    }
}

void SubReactor::stop() {
    if (running_) {
        running_ = false;
        if (thread_ && thread_->joinable()) {
            thread_->join();
        }
    }
}

void SubReactor::addConnection(Handle fd) {
    auto conn = std::make_shared<ConnectionHandler>(fd);
    
    // 设置消息回调（这里实现echo功能）
    conn->setMessageCallback([](const char* data, size_t len) {
        std::cout << "Received " << len << " bytes" << std::endl;
    });
    
    // 设置关闭回调
    conn->setCloseCallback([this](Handle fd) {
        connections_.erase(fd);
    });
    
    connections_[fd] = conn;
    reactor_.registerHandler(conn.get(), ReadEvent);
}

void SubReactor::threadFunc() {
    while (running_) {
        reactor_.eventLoop(100); // 100ms timeout
    }
}

} 