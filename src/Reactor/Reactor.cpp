#include "Reactor/Reactor.h"
#include "Reactor/ReactorImplementation.h"

namespace Reactor {

Reactor::Reactor() : impl_(std::make_unique<ReactorImplementation>()) {}

Reactor::~Reactor() = default;

int Reactor::registerHandler(EventHandler* handler, EventType event) {
    return impl_->registerHandler(handler, event);
}

void Reactor::removeHandler(EventHandler* handler) {
    impl_->removeHandler(handler);
}

void Reactor::modifyHandler(EventHandler* handler, EventType event) {
    impl_->removeHandler(handler);
    impl_->registerHandler(handler, event);
}

void Reactor::eventLoop(int timeout) {
    impl_->eventLoop(timeout);
}

} // namespace Reactor