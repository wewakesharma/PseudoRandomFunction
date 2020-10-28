cmake_minimum_required(VERSION 3.16)
project(darkmattercode)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/darkmatter)

add_executable(darkmattercode
        include/darkmatter/mains.hpp
        include/darkmatter/OT.hpp
        include/darkmatter/packed_PRF_central.h
        include/darkmatter/packedMod2.hpp
        include/darkmatter/packedMod3.hpp
        include/darkmatter/PRF.hpp
        include/darkmatter/Timing.hpp
        include/darkmatter/Toeplitz-by-x.hpp
        include/darkmatter/utils.hpp
        src/OT.cpp
        src/packed_PRF_central.cpp
        src/PRF.cpp
        src/Toeplitz-by-x.cpp
        tests/test_OT.cpp
        tests/test_Packed_PRF.cpp
        tests/test_packedMod2.cpp
        tests/test_packedMod3.cpp
        tests/test_PRF.cpp
        tests/test_Toeplitz-by-x.cpp src/unpacked_PRF_central.cpp include/darkmatter/unpacked_PRF_central.h tests/test_unpacked_PRF.cpp)
