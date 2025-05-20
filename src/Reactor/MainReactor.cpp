#include "Reactor/MainReactor.h"
#include <iostream>

namespace Reactor {

MainReactor::MainReactor(int port, size_t subReactorCount) {
    // 创建Acceptor
    acceptor_ = std::make_unique<Acceptor>(reactor_, port); // 什么时候用unique_ptr，因为 acceptor 的生命周期完全由 mainReactor 管理，所以用 unique_ptr
    
    // 创建SubReactors
    for (size_t i = 0; i < subReactorCount; ++i) {
        auto subReactor = std::make_shared<SubReactor>();   // 为什么用shared_ptr，需要多个地方共享 SubReactor 的实例
        // subReactors_.push_back(subReactor);  // 使用 push_back, 会调用 SubReactor 的拷贝构造函数，如果用 unique_ptr，会调用移动构造函数
        // subReactors_.push_back(std::move(subReactor));  // 使用 move，会调用移动构造函数
        // subReactors_.push_back(std::make_shared<SubReactor>());  // 使用 make_shared，会调用拷贝构造函数
        subReactors_.emplace_back(subReactor);  // 使用 emplace_back，会调用移动构造函数
    }
    
    // 设置新连接回调
    acceptor_->setNewConnectionCallback([this](Handle fd) {   // 使用lambda表达式，捕获this指针，避免内存泄漏
        distributeConnection(fd);
    });
}

MainReactor::~MainReactor() {
    stop();
}

void MainReactor::start() {
    if (!running_) {
        running_ = true;
        
        // 启动所有 SubReactor
        for (auto& subReactor : subReactors_) {
            subReactor->start();
        }
        
        // 注册 Acceptor
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