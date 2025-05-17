#include "Reactor/Reactor.h"

namespace Reactor {

Reactor::Reactor() : impl_(std::make_unique<ReactorImplementation>()) {}

Reactor& Reactor::getInstance() {
    static Reactor instance;  // 保证线程安全的单例(C++11及以上)
    return instance;
}

int Reactor::registerHandler(EventHandler* handler, EventType event) {
    return impl_->registerHandler(handler, event);
}

void Reactor::removeHandler(EventHandler* handler) {
    impl_->removeHandler(handler);
}

void Reactor::eventLoop(int timeout) {
    impl_->eventLoop(timeout);
}

// 其他实现方法...
} // namespace Reactor