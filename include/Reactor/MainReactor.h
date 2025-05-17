#pragma once

#include "Reactor.h"
#include "Acceptor.h"
#include "SubReactor.h"
#include <vector>
#include <memory>

namespace Reactor {

class MainReactor {
public:
    explicit MainReactor(int port, size_t subReactorCount = 4);
    ~MainReactor();

    void start();
    void stop();

private:
    void distributeConnection(Handle fd);

    Reactor reactor_;
    std::unique_ptr<Acceptor> acceptor_;
    std::vector<SubReactorPtr> subReactors_;
    size_t nextReactor_{0};
    bool running_{false};
};

} 