cmake_minimum_required(VERSION 3.16)
project(2PC)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(include)
include_directories(include/darkmatter)
include_directories(tests)

add_executable(2PC
        include/darkmatter/OT.hpp
        include/darkmatter/packedMod2.hpp
        include/darkmatter/packedMod3.hpp
        include/darkmatter/Toeplitz-by-x.hpp
        include/darkmatter/utils.hpp
        "src/OT copy.cpp"
        src/OT.cpp
        src/Toeplitz-by-x.cpp
        src/Toeplitz-by-x_p1.cpp
        src/Toeplitz-by-x_p2.cpp
        tests/mains.h
        tests/test_OT.cpp
        tests/test_packedMod2.cpp
        tests/test_packedMod3.cpp
        tests/test_Toeplitz-by-x.cpp src/data_sender.cpp include/darkmatter/data_sender.h src/data_receiver.cpp include/darkmatter/data_receiver.h)
