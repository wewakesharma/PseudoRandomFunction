cmake_minimum_required(VERSION 3.16)
project(scunittest)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_subdirectory(include EXCLUDE_FROM_ALL)

add_executable(scunittest
        pi_unit_test.cpp
        include/pi23protpacked.cpp
        include/pi23protpacked.h
        include/utils.cpp
        include/utils.h
        pi_unit_test.h
        piprot_test.cpp)
