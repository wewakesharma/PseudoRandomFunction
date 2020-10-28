cmake_minimum_required(VERSION 3.12)
project(2PC)

set(CMAKE_CXX_STANDARD 14)

include_directories(comm)
include_directories(include)
include_directories(include/darkmatter)

add_executable(2PC
        comm/client.cpp
        comm/recv.cpp
        comm/recv.h
        comm/server.cpp
        comm/snd.cpp
        comm/snd.h
        include/darkmatter/data_receiver.h
        include/darkmatter/data_sender.h
        include/darkmatter/mains.hpp
        include/darkmatter/OT.hpp
        include/darkmatter/packedMod2.hpp
        include/darkmatter/packedMod3.hpp
        include/darkmatter/Toeplitz-by-x.hpp
        include/darkmatter/utils.hpp
        src/data_receiver.cpp
        src/data_sender.cpp
        src/OT.cpp
        src/Toeplitz-by-x.cpp
        src/Toeplitz-by-x_p1.cpp
        src/Toeplitz-by-x_p2.cpp
        tests/test_OT.cpp
        tests/test_packedMod2.cpp
        tests/test_packedMod3.cpp
        tests/test_Toeplitz-by-x.cpp)
