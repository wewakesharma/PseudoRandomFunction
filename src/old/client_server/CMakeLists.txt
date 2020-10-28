cmake_minimum_required(VERSION 3.16)
project(client_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(client_server
        client.cpp
        server.cpp
        test.cpp)
