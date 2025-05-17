#include "Reactor/MainReactor.h"
#include <iostream>
#include <thread>

using namespace Reactor;

int main() {
    try {
        // 创建主Reactor，使用4个从Reactor
        MainReactor mainReactor(8080, 4);
        
        std::cout << "Server starting on port 8080..." << std::endl;
        mainReactor.start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}