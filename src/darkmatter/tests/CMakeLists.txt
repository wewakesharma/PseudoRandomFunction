cmake_minimum_required(VERSION 3.16)
project(tests)

set(CMAKE_CXX_STANDARD 14)

add_executable(tests
        test_packedMod2.cpp
        test_packedMod3.cpp
        test_Toeplitz-by-x.cpp)
