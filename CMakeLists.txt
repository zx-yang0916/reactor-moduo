cmake_minimum_required(VERSION 3.10)
project(ReactorServer)

set(CMAKE_CXX_STANDARD 17)

# 检查epoll支持
include(CheckFunctionExists)
check_function_exists(epoll_create HAVE_EPOLL)

if(HAVE_EPOLL)
    add_definitions(-DHAVE_EPOLL)
    message(STATUS "System supports epoll")
else()
    message(STATUS "System does not support epoll, will use select")
endif()

# 包含目录
include_directories(include)

# 源文件
file(GLOB SOURCES src/*.cpp src/Reactor/*.cpp)

# 可执行文件
add_executable(reactor_server ${SOURCES})

# 链接库
target_link_libraries(reactor_server pthread)