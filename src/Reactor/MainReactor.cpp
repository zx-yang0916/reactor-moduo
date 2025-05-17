#include "Reactor/MainReactor.h"
#include <iostream>

namespace Reactor {

MainReactor::MainReactor(int port, size_t subReactorCount) {
    // 创建Acceptor
    acceptor_ = std::make_unique<Acceptor>(reactor_, port);
    
    // 创建SubReactors
    for (size_t i = 0; i < subReactorCount; ++i) {
        auto subReactor = std::make_shared<SubReactor>();
        subReactors_.push_back(subReactor);
    }
    
    // 设置新连接回调
    acceptor_->setNewConnectionCallback([this](Handle fd) {
        distributeConnection(fd);
    });
}

MainReactor::~MainReactor() {
    stop();
}

void MainReactor::start() {
    if (!running_) {
        running_ = true;
        
        // 启动所有SubReactor
        for (auto& subReactor : subReactors_) {
            subReactor->start();
        }
        
        // 注册Acceptor
        reactor_.registerHandler(acceptor_.get(), ReadEvent);
        std::cout << "Server started with " << subReactors_.size() << " SubReactors" << std::endl;
        
        // 开始主事件循环
        while (running_) {
            reactor_.eventLoop(100); // 100ms timeout
        }
    }
}

void MainReactor::stop() {
    if (running_) {
        running_ = false;
        
        // 停止所有SubReactor
        for (auto& subReactor : subReactors_) {
            subReactor->stop();
        }
    }
}

void MainReactor::distributeConnection(Handle fd) {
    // 轮询分发
    subReactors_[nextReactor_]->addConnection(fd);
    nextReactor_ = (nextReactor_ + 1) % subReactors_.size();
}

} 