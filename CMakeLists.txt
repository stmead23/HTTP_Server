cmake_minimum_required(VERSION 3.13)
project(http-server-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

set(SOURCE_FILES src/server.cpp)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
ADD_EXECUTABLE(thread_1 thread_1.cpp)

add_executable(server ${SOURCE_FILES})
