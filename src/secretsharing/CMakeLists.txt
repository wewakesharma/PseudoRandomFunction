cmake_minimum_required(VERSION 3.16)
project(wordpacked)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(wordpacked
    dmweakPRF.cpp
    dmweakPRF.h
    dmweakPRFnoPack.h
    dmweakPRFnpack.cpp
    pi23prot.cpp
    pi23prot.h
    pi23protpacked.cpp
    prfTiming.cpp)
