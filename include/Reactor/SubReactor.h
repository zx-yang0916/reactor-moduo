#pragma once

#include "Reactor.h"
#include "ConnectionHandler.h"
#include <thread>
#include <memory>
#include <atomic>
#include <unordered_map>

namespace Reactor {

class SubReactor {
public:
    SubReactor();
    ~SubReactor();

    void start();
    void stop();
    void addConnection(Handle fd);

private:
    void threadFunc();

    Reactor reactor_;
    std::unique_ptr<std::thread> thread_;
    std::atomic<bool> running_{false};
    std::unordered_map<Handle, ConnectionHandlerPtr> connections_;
};

using SubReactorPtr = std::shared_ptr<SubReactor>;

} 