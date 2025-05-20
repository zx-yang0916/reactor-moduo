#pragma once

#include "Reactor.h"
#include "Acceptor.h"   // 什么时候用前向声明，什么时候用 include？
#include "SubReactor.h"
#include <vector>
#include <memory>
#include <atomic>

namespace Reactor {

class MainReactor {
public:
    explicit MainReactor(int port, size_t subReactorCount = 4);     // explicit 禁止隐式转换
    ~MainReactor();

    void start();
    void stop();

private:
    void distributeConnection(Handle fd);   // 分发连接

    Reactor reactor_;
    std::unique_ptr<Acceptor> acceptor_;    // 接受连接的 acceptor
    std::vector<SubReactorPtr> subReactors_;    // 从 reactor
    size_t nextReactor_{0};    // 下一个从 reactor
    std::atomic<bool> running_{false};    // 是否运行 
};

} 