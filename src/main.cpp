#include "Reactor/MainReactor.h"
#include <iostream>
#include <thread>


int main() {
    try {
        Reactor::MainReactor mainReactor(8080, 4);   // 一个主 reactor， 4个从 reactor
        
        std::cout << "Server starting on port 8080..." << std::endl;
        mainReactor.start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}