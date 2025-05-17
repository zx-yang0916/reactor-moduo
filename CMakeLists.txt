cmake_minimum_required(VERSION 3.10)
project(reactor_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查epoll支持
include(CheckSymbolExists)
check_symbol_exists(epoll_create "sys/epoll.h" HAVE_EPOLL)

if(HAVE_EPOLL)
    message(STATUS "System supports epoll")
    add_definitions(-DHAVE_EPOLL)
else()
    message(STATUS "System does not support epoll, using select")
endif()

# 包含目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件列表
set(SOURCES
    src/main.cpp
    src/Reactor/Reactor.cpp
    src/Reactor/ReactorImplementation.cpp
    src/Reactor/SelectDemultiplexer.cpp
    src/Reactor/Acceptor.cpp
    src/Reactor/ConnectionHandler.cpp
    src/Reactor/MainReactor.cpp
    src/Reactor/SubReactor.cpp
)

# 如果支持epoll，添加epoll实现
if(HAVE_EPOLL)
    list(APPEND SOURCES src/Reactor/EpollDemultiplexer.cpp)
endif()

# 可执行文件
add_executable(reactor_server ${SOURCES})

# 链接库
target_link_libraries(reactor_server pthread)