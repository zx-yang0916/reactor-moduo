#pragma once

#include "EventHandler.h"
#include <memory>
// #include "ReactorImplementation.h" 

namespace Reactor {

class ReactorImplementation;  // 移到类外面

class Reactor {
public:
    Reactor();
    ~Reactor();

    int registerHandler(EventHandler* handler, EventType event);    // 注册事件
    void removeHandler(EventHandler* handler);                      // 移除事件
    void modifyHandler(EventHandler* handler, EventType event);     // 修改事件
    void eventLoop(int timeout = 10);                               // 事件循环，即轮询事件

private:
    Reactor(const Reactor&) = delete;                                // 禁止拷贝构造
    Reactor& operator=(const Reactor&) = delete;                     // 禁止赋值

    // class ReactorImplementation;     // 声明在此处为嵌套类，再进行实现时需要 Reactor::ReactorImplementation 的形式
    std::unique_ptr<ReactorImplementation> impl_;                   // 实现类，PIMPL 设计模式
    // ReactorImplementation* impl_;
};

} // namespace Reactor