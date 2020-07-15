cmake_minimum_required(VERSION 3.16)
project(scunittest)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(scunittest
        dmweakPRFpacked.cpp
        dmweakPRFpacked.h
        pi23protpacked.cpp
        pi23protpacked.h
        pi_unit_test.cpp
        pi_unit_test.h
        piprot_test.cpp
        utils.cpp
        utils.h)
