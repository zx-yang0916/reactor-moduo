#include "Reactor/Reactor.h"
#include "Reactor/ListenHandle.h"
#include <iostream>

using namespace Reactor;

int main() {
    try {
        Reactor::Reactor& reactor = Reactor::Reactor::getInstance();
        ListenHandle listener(reactor, 8080);
        
        reactor.registerHandler(&listener, ReadEvent);
        std::cout << "Server started on port 8080..." << std::endl;
        
        reactor.eventLoop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}