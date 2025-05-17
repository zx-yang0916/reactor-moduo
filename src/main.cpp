#include "Reactor/Reactor.h"
#include "Reactor/Acceptor.h"
#include "Reactor/ConnectionHandler.h"
#include <iostream>
#include <memory>
#include <unordered_map>

using namespace Reactor;

int main() {
    try {
        Reactor::Reactor& reactor = Reactor::Reactor::getInstance();
        
        // 创建连接管理器
        std::unordered_map<Handle, ConnectionHandlerPtr> connections;
        
        // 创建Acceptor
        auto acceptor = std::make_unique<Acceptor>(reactor, 8080);
        
        // 设置新连接回调
        acceptor->setNewConnectionCallback([&](Handle fd) {
            auto conn = std::make_shared<ConnectionHandler>(fd);
            
            // 设置消息回调（这里实现echo功能）
            conn->setMessageCallback([](const char* data, size_t len) {
                std::cout << "Received " << len << " bytes" << std::endl;
            });
            
            // 设置关闭回调
            conn->setCloseCallback([&connections](Handle fd) {
                connections.erase(fd);
            });
            
            connections[fd] = conn;
            reactor.registerHandler(conn.get(), ReadEvent);
        });
        
        // 注册Acceptor
        reactor.registerHandler(acceptor.get(), ReadEvent);
        std::cout << "Server started on port 8080..." << std::endl;
        
        // 开始事件循环
        reactor.eventLoop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}