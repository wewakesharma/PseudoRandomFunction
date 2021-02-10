cmake_minimum_required(VERSION 3.16)
project(newprotocol)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/darkmatter)

add_executable(newprotocol
        include/darkmatter/integer_pack.h
        include/darkmatter/lookup_functions.h
        include/darkmatter/mains.hpp
        include/darkmatter/newprotocol.hpp
        include/darkmatter/newprotocol_test.hpp
        include/darkmatter/OPRF.h
        include/darkmatter/OT.hpp
        include/darkmatter/packed_PRF_central.h
        include/darkmatter/packedMod2.hpp
        include/darkmatter/packedMod3.hpp
        include/darkmatter/PRF.hpp
        include/darkmatter/Timing.hpp
        include/darkmatter/Toeplitz-by-x.hpp
        include/darkmatter/unpacked_PRF_central.h
        include/darkmatter/utils.hpp
        src/integer_pack.cpp
        src/lookup_functions.cpp
        src/newprotocol.cpp
        src/OPRF.cpp
        src/OT.cpp
        src/packed_PRF_central.cpp
        src/PRF.cpp
        src/Toeplitz-by-x.cpp
        src/unpacked_PRF_central.cpp
        tests/test_central_int_packed.cpp
        tests/test_dmprotocol.cpp
        tests/test_independent_lookup.cpp
        tests/test_lookup.cpp
        tests/test_newprotocol.cpp
        tests/test_oprf.cpp
        tests/test_OT.cpp
        tests/test_Packed_PRF.cpp
        tests/test_packed_PRF_central_lookup.cpp
        tests/test_packedMod2.cpp
        tests/test_packedMod3.cpp
        tests/test_PRF.cpp
        tests/test_Toeplitz-by-x.cpp
        tests/test_unit_newprotocol.cpp
        tests/test_unpacked_PRF.cpp tests/test_unpacked_phase1.cpp)
