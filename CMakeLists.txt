cmake_minimum_required(VERSION 3.13)
project(http-server-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard
find_package(Threads REQUIRED)

set(SOURCE_FILES src/server.cpp)

add_executable(server ${SOURCE_FILES})
target_link_libraries(Threads::Threads)
