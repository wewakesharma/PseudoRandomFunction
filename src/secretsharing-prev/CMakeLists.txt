cmake_minimum_required(VERSION 3.16)
project(secretsharing)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

<<<<<<< HEAD
add_executable(secretsharing
        dmweakPRF.cpp
        dmweakPRF.h
        dmweakPRFpacked.cpp
        dmweakPRFpacked.h
        pi23prot.cpp
        pi23prot.h
        pi23protpacked.cpp
        pi23protpacked.h
        secShareTiming.cpp
        utils.cpp
        utils.h)
=======
add_executable(wordpacked
    dmweakPRF.cpp
    dmweakPRF.h
    dmweakPRFnoPack.h
    dmweakPRFnopack.cpp
    pi23prot.cpp
    pi23prot.h
    pi23protpacked.cpp pi23protpacked.h piprot_test.cpp piprot_unpck_test.cpp)
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8
