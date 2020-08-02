cmake_minimum_required(VERSION 3.16)
project(darkmatter)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/darkmatter)
include_directories(tests)

add_executable(darkmatter
        include/darkmatter/packedMod2.hpp
        include/darkmatter/packedMod3.hpp
        include/darkmatter/Toeplitz-by-x.hpp
        include/darkmatter/utils.hpp
        src/OT.cpp
        src/Toeplitz-by-x.cpp
        tests/mains.h
        tests/test_OT.cpp
        tests/test_packedMod2.cpp
        tests/test_packedMod3.cpp
        tests/test_Toeplitz-by-x.cpp)
