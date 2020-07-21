cmake_minimum_required(VERSION 3.16)
project(secretsharing)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(secretsharing
        dmweakPRF.cpp
        dmweakPRF.h
        dmweakPRFpacked.cpp
        dmweakPRFpacked.h
        mains.h
        pi23prot.cpp
        pi23prot.h
        pi23protpacked.cpp
        pi23protpacked.h
        pi_unit_test.cpp
        pi_unit_test.h
        piprot_test.cpp
        secShareTiming.cpp
        secShareTiming.h
        secShareTimingpacked.cpp
        secShareTimingpacked.h
        utils.cpp
        utils.h
        utilspacked.cpp
        utilspacked.h)
