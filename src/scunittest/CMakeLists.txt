cmake_minimum_required(VERSION 3.16)
project(scunittest)

set(CMAKE_CXX_STANDARD 14)

include_directories( ${MY_EXTERNAL_SOURCE__DIR})

add_executable(scunittest
        pi23protpacked.cpp
        pi23protpacked.h
        pi_unit_test.cpp
        pi_unit_test.h
        utils.cpp
        utils.h
        piprot_test.cpp)
