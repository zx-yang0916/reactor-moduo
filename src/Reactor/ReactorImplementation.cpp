#include "Reactor/ReactorImplementation.h"
#include <memory>
#ifdef HAVE_EPOLL
#include "Reactor/EpollDemultiplexer.h"
#else
#include "Reactor/SelectDemultiplexer.h"
#endif
#include <iostream>

namespace Reactor {

ReactorImplementation::ReactorImplementation() {
#ifdef HAVE_EPOLL
    demultiplexer_ = std::make_unique<EpollDemultiplexer>();
#else
    demultiplexer_ = std::make_unique<SelectDemultiplexer>();
#endif
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