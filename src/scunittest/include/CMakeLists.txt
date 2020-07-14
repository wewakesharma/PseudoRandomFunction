cmake_minimum_required(VERSION 3.16)
project(include_test)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)


add_executable(include_test
        pi23protpacked.cpp
        pi23protpacked.h
        utils.cpp
        utils.h)