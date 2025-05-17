#include "Reactor/ReactorImplementation.h"
#include <memory>
#include "Reactor/EpollDemultiplexer.h"
#include <iostream>


namespace Reactor {

ReactorImplementation::ReactorImplementation() {
    demultiplexer_ = std::make_unique<EpollDemultiplexer>();
}

ReactorImplementation::~ReactorImplementation() = default;

int ReactorImplementation::registerHandler(EventHandler* handler, EventType event) {
    handlers_[handler->getHandle()] = handler;
    return demultiplexer_->registerHandler(handler, event);
}

void ReactorImplementation::removeHandler(EventHandler* handler) {
    handlers_.erase(handler->getHandle());
    demultiplexer_->removeHandler(handler->getHandle());
}

void ReactorImplementation::modifyHandler(EventHandler* handler, EventType event) {
    demultiplexer_->removeHandler(handler->getHandle());
    demultiplexer_->registerHandler(handler, event);
}

void ReactorImplementation::eventLoop(int timeout) {
    while(true) {
        int num_events = demultiplexer_->waitEvent(handlers_, timeout);
        if (num_events < 0) {
            std::cerr << "Error in event loop" << std::endl;
            break;
        }
    }
}

} // namespace