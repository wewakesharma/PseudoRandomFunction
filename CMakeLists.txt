cmake_minimum_required(VERSION 3.16)
project(PseudoRandomFunction)

set(CMAKE_CXX_STANDARD 14)

include_directories(archivedcode/darkmatter_time/include)
include_directories(archivedcode/darkmatter_time/include/darkmatter)
include_directories(archivedcode/darkmattercode/include)
include_directories(archivedcode/darkmattercode/include/darkmatter)
include_directories(archivedcode/old/testing_old)
include_directories(misc/test/test)
include_directories(src/2PC)
include_directories(src/2PC/include)
include_directories(src/2PC/include/darkmatter)
include_directories(src/darkmattercode/include)
include_directories(src/darkmattercode/include/darkmatter)
include_directories("src/darkmattercode2 copy/include")
include_directories("src/darkmattercode2 copy/include/darkmatter")
include_directories(src/darkmattercodeprevunused/include)
include_directories(src/darkmattercodeprevunused/include/darkmatter)
include_directories(src/newprotocol/include)
include_directories(src/newprotocol/include/darkmatter)
include_directories("src/newprotocol copy/include")
include_directories("src/newprotocol copy/include/darkmatter")
include_directories(src/newprotocolprev/include)
include_directories(src/newprotocolprev/include/darkmatter)
include_directories(src/newprotocolprev2/include)
include_directories(src/newprotocolprev2/include/darkmatter)
include_directories(src/newprotocoltemp/include)
include_directories(src/newprotocoltemp/include/darkmatter)
include_directories("src/old/2PC copy/comm")
include_directories("src/old/2PC copy/include")
include_directories("src/old/2PC copy/include/darkmatter")
include_directories("src/old/darkmattercode copy/include")
include_directories("src/old/darkmattercode copy/include/darkmatter")
include_directories("src/old/darkmattercode copy 2/include")
include_directories("src/old/darkmattercode copy 2/include/darkmatter")
include_directories("src/old/secretsharing copy")
include_directories(src/PseudoRandomFunction/archivedcode/darkmatter_time/include)
include_directories(src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter)
include_directories(src/PseudoRandomFunction/archivedcode/darkmattercode/include)
include_directories(src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter)
include_directories(src/PseudoRandomFunction/archivedcode/old/testing_old)
include_directories(src/PseudoRandomFunction/misc/test/test)
include_directories(src/PseudoRandomFunction/src/darkmattercode/include)
include_directories(src/PseudoRandomFunction/src/darkmattercode/include/darkmatter)
include_directories(src/PseudoRandomFunction/src/newprotocol/include)
include_directories(src/PseudoRandomFunction/src/newprotocol/include/darkmatter)
include_directories(src/PseudoRandomFunction/src/scunittest)
include_directories(src/PseudoRandomFunction/src/secretsharing)
include_directories(src/PseudoRandomFunction/src/testing)
include_directories(src/PseudoRandomFunction/src/two_party_comm2/comm)
include_directories(src/PseudoRandomFunction/src/two_party_comm2/include)
include_directories(src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter)
include_directories(src/PseudoRandomFunction/src/unitTesting)
include_directories(src/scunittest)
include_directories(src/secretsharing)
include_directories(src/testing)
include_directories(src/two_party_comm2/comm)
include_directories(src/two_party_comm2/include)
include_directories(src/two_party_comm2/include/darkmatter)
include_directories(src/unitTesting)

add_executable(PseudoRandomFunction
        archivedcode/client_server_chat/client.cpp
        archivedcode/client_server_chat/server.cpp
        archivedcode/darkmatter_time/include/darkmatter/mains.hpp
        archivedcode/darkmatter_time/include/darkmatter/OT.hpp
        archivedcode/darkmatter_time/include/darkmatter/packedMod2.hpp
        archivedcode/darkmatter_time/include/darkmatter/packedMod3.hpp
        archivedcode/darkmatter_time/include/darkmatter/PRF.hpp
        archivedcode/darkmatter_time/include/darkmatter/Timing.hpp
        archivedcode/darkmatter_time/include/darkmatter/Toeplitz-by-x.hpp
        archivedcode/darkmatter_time/include/darkmatter/utils.hpp
        archivedcode/darkmatter_time/src/OT.cpp
        archivedcode/darkmatter_time/src/PRF.cpp
        archivedcode/darkmatter_time/src/Toeplitz-by-x.cpp
        archivedcode/darkmatter_time/tests/test_OT.cpp
        archivedcode/darkmatter_time/tests/test_packedMod2.cpp
        archivedcode/darkmatter_time/tests/test_packedMod3.cpp
        archivedcode/darkmatter_time/tests/test_PRF.cpp
        archivedcode/darkmatter_time/tests/test_Toeplitz-by-x.cpp
        archivedcode/darkmattercode/include/darkmatter/mains.hpp
        archivedcode/darkmattercode/include/darkmatter/OT.hpp
        archivedcode/darkmattercode/include/darkmatter/packed_PRF_central.h
        archivedcode/darkmattercode/include/darkmatter/packedMod2.hpp
        archivedcode/darkmattercode/include/darkmatter/packedMod3.hpp
        archivedcode/darkmattercode/include/darkmatter/PRF.hpp
        archivedcode/darkmattercode/include/darkmatter/Timing.hpp
        archivedcode/darkmattercode/include/darkmatter/Toeplitz-by-x.hpp
        archivedcode/darkmattercode/include/darkmatter/unpacked_PRF_central.h
        archivedcode/darkmattercode/include/darkmatter/utils.hpp
        archivedcode/darkmattercode/src/OT.cpp
        archivedcode/darkmattercode/src/packed_PRF_central.cpp
        archivedcode/darkmattercode/src/PRF.cpp
        archivedcode/darkmattercode/src/Toeplitz-by-x.cpp
        archivedcode/darkmattercode/src/unpacked_PRF_central.cpp
        archivedcode/darkmattercode/tests/test_OT.cpp
        archivedcode/darkmattercode/tests/test_Packed_PRF.cpp
        archivedcode/darkmattercode/tests/test_packedMod2.cpp
        archivedcode/darkmattercode/tests/test_packedMod3.cpp
        archivedcode/darkmattercode/tests/test_PRF.cpp
        archivedcode/darkmattercode/tests/test_Toeplitz-by-x.cpp
        archivedcode/darkmattercode/tests/test_unpacked_PRF.cpp
        archivedcode/naive/matrixvecmul.cpp
        archivedcode/naive/matvecTiming.cpp
        archivedcode/naive/optmatvec.cpp
        archivedcode/naive/p1_test.cpp
        archivedcode/naive/pack_unpack.cpp
        archivedcode/naive/phase3.cpp
        archivedcode/naive/phase3_grg.cpp
        archivedcode/naive/pi_protocol.cpp
        archivedcode/naive/prf.cpp
        archivedcode/naive/PRF_naive.cpp
        archivedcode/naive/PRF_test.cpp
        archivedcode/naive/unit_edit.cpp
        archivedcode/old/piprotocol/pi_protocol.cpp
        "archivedcode/old/testing_old/unit_test copy.cpp"
        archivedcode/old/testing_old/unit_test.cpp
        archivedcode/old/testing_old/weak23prf.cpp
        archivedcode/old/testing_old/weak23prf.h
        archivedcode/old/matrixvecmul.cpp
        archivedcode/old/matvecTiming.cpp
        archivedcode/old/pi_protocol.cpp
        archivedcode/old/prf.cpp
        archivedcode/mat_vec_toeplitz.cpp
        archivedcode/matrixvecmul.cpp
        archivedcode/matvecTiming.cpp
        archivedcode/pi_protocol.cpp
        archivedcode/prf.cpp
        archivedcode/PRF_naive.cpp
        archivedcode/prf_naive2.cpp
        misc/test/test/test.h
        misc/test/test/test.m
        src/2PC/include/darkmatter/mains.hpp
        src/client_server/test.cpp
        src/communication_example/client.cpp
        src/communication_example/server.cpp
        src/darkmattercode/include/darkmatter/mains.hpp
        src/darkmattercode/include/darkmatter/OT.hpp
        src/darkmattercode/include/darkmatter/packed_PRF_central.h
        src/darkmattercode/include/darkmatter/packedMod2.hpp
        src/darkmattercode/include/darkmatter/packedMod3.hpp
        src/darkmattercode/include/darkmatter/PRF.hpp
        src/darkmattercode/include/darkmatter/Timing.hpp
        src/darkmattercode/include/darkmatter/Toeplitz-by-x.hpp
        src/darkmattercode/include/darkmatter/unpacked_PRF_central.h
        src/darkmattercode/include/darkmatter/utils.hpp
        src/darkmattercode/src/OT.cpp
        src/darkmattercode/src/packed_PRF_central.cpp
        src/darkmattercode/src/PRF.cpp
        src/darkmattercode/src/Toeplitz-by-x.cpp
        src/darkmattercode/src/unpacked_PRF_central.cpp
        src/darkmattercode/tests/test_OT.cpp
        src/darkmattercode/tests/test_Packed_PRF.cpp
        src/darkmattercode/tests/test_packedMod2.cpp
        src/darkmattercode/tests/test_packedMod3.cpp
        src/darkmattercode/tests/test_PRF.cpp
        src/darkmattercode/tests/test_Toeplitz-by-x.cpp
        src/darkmattercode/tests/test_unpacked_PRF.cpp
        "src/darkmattercode2 copy/include/darkmatter/mains.hpp"
        "src/darkmattercode2 copy/include/darkmatter/OT.hpp"
        "src/darkmattercode2 copy/include/darkmatter/packed_PRF_central.h"
        "src/darkmattercode2 copy/include/darkmatter/packedMod2.hpp"
        "src/darkmattercode2 copy/include/darkmatter/packedMod3.hpp"
        "src/darkmattercode2 copy/include/darkmatter/PRF.hpp"
        "src/darkmattercode2 copy/include/darkmatter/Timing.hpp"
        "src/darkmattercode2 copy/include/darkmatter/Toeplitz-by-x.hpp"
        "src/darkmattercode2 copy/include/darkmatter/utils.hpp"
        "src/darkmattercode2 copy/src/OT.cpp"
        "src/darkmattercode2 copy/src/packed_PRF_central.cpp"
        "src/darkmattercode2 copy/src/PRF.cpp"
        "src/darkmattercode2 copy/src/Toeplitz-by-x.cpp"
        "src/darkmattercode2 copy/tests/test_OT.cpp"
        "src/darkmattercode2 copy/tests/test_Packed_PRF.cpp"
        "src/darkmattercode2 copy/tests/test_packedMod2.cpp"
        "src/darkmattercode2 copy/tests/test_packedMod3.cpp"
        "src/darkmattercode2 copy/tests/test_PRF.cpp"
        "src/darkmattercode2 copy/tests/test_Toeplitz-by-x.cpp"
        src/darkmattercodeprevunused/include/darkmatter/mains.hpp
        src/darkmattercodeprevunused/include/darkmatter/OT.hpp
        src/darkmattercodeprevunused/include/darkmatter/packed_PRF_central.h
        src/darkmattercodeprevunused/include/darkmatter/packedMod2.hpp
        src/darkmattercodeprevunused/include/darkmatter/packedMod3.hpp
        src/darkmattercodeprevunused/include/darkmatter/PRF.hpp
        src/darkmattercodeprevunused/include/darkmatter/Timing.hpp
        src/darkmattercodeprevunused/include/darkmatter/Toeplitz-by-x.hpp
        src/darkmattercodeprevunused/include/darkmatter/unpacked_PRF_central.h
        src/darkmattercodeprevunused/include/darkmatter/utils.hpp
        src/darkmattercodeprevunused/src/OT.cpp
        src/darkmattercodeprevunused/src/packed_PRF_central.cpp
        src/darkmattercodeprevunused/src/PRF.cpp
        src/darkmattercodeprevunused/src/Toeplitz-by-x.cpp
        src/darkmattercodeprevunused/src/unpacked_PRF_central.cpp
        src/darkmattercodeprevunused/tests/test_OT.cpp
        src/darkmattercodeprevunused/tests/test_Packed_PRF.cpp
        src/darkmattercodeprevunused/tests/test_packedMod2.cpp
        src/darkmattercodeprevunused/tests/test_packedMod3.cpp
        src/darkmattercodeprevunused/tests/test_PRF.cpp
        src/darkmattercodeprevunused/tests/test_Toeplitz-by-x.cpp
        src/darkmattercodeprevunused/tests/test_unpacked_PRF.cpp
        src/newprotocol/include/darkmatter/lookup_functions.h
        src/newprotocol/include/darkmatter/mains.hpp
        src/newprotocol/include/darkmatter/newprotocol.hpp
        src/newprotocol/include/darkmatter/newprotocol_test.hpp
        src/newprotocol/include/darkmatter/OT.hpp
        src/newprotocol/include/darkmatter/packed_PRF_central.h
        src/newprotocol/include/darkmatter/packedMod2.hpp
        src/newprotocol/include/darkmatter/packedMod3.hpp
        src/newprotocol/include/darkmatter/PRF.hpp
        src/newprotocol/include/darkmatter/Timing.hpp
        src/newprotocol/include/darkmatter/Toeplitz-by-x.hpp
        src/newprotocol/include/darkmatter/unpacked_PRF_central.h
        src/newprotocol/include/darkmatter/utils.hpp
        src/newprotocol/src/lookup_functions.cpp
        src/newprotocol/src/newprotocol.cpp
        src/newprotocol/src/OT.cpp
        src/newprotocol/src/packed_PRF_central.cpp
        src/newprotocol/src/PRF.cpp
        src/newprotocol/src/Toeplitz-by-x.cpp
        src/newprotocol/src/unpacked_PRF_central.cpp
        src/newprotocol/tests/test_dmprotocol.cpp
        src/newprotocol/tests/test_lookup.cpp
        src/newprotocol/tests/test_newprotocol.cpp
        src/newprotocol/tests/test_OT.cpp
        src/newprotocol/tests/test_Packed_PRF.cpp
        src/newprotocol/tests/test_packedMod2.cpp
        src/newprotocol/tests/test_packedMod3.cpp
        src/newprotocol/tests/test_PRF.cpp
        src/newprotocol/tests/test_Toeplitz-by-x.cpp
        src/newprotocol/tests/test_unit_newprotocol.cpp
        src/newprotocol/tests/test_unpacked_PRF.cpp
        "src/newprotocol copy/include/darkmatter/mains.hpp"
        "src/newprotocol copy/include/darkmatter/newprotocol.hpp"
        "src/newprotocol copy/include/darkmatter/newprotocol_test.hpp"
        "src/newprotocol copy/include/darkmatter/OT.hpp"
        "src/newprotocol copy/include/darkmatter/packed_PRF_central.h"
        "src/newprotocol copy/include/darkmatter/packedMod2.hpp"
        "src/newprotocol copy/include/darkmatter/packedMod3.hpp"
        "src/newprotocol copy/include/darkmatter/PRF.hpp"
        "src/newprotocol copy/include/darkmatter/Timing.hpp"
        "src/newprotocol copy/include/darkmatter/Toeplitz-by-x.hpp"
        "src/newprotocol copy/include/darkmatter/unpacked_PRF_central.h"
        "src/newprotocol copy/include/darkmatter/utils.hpp"
        "src/newprotocol copy/src/newprotocol.cpp"
        "src/newprotocol copy/src/OT.cpp"
        "src/newprotocol copy/src/packed_PRF_central.cpp"
        "src/newprotocol copy/src/PRF.cpp"
        "src/newprotocol copy/src/Toeplitz-by-x.cpp"
        "src/newprotocol copy/src/unpacked_PRF_central.cpp"
        "src/newprotocol copy/tests/test_dmprotocol.cpp"
        "src/newprotocol copy/tests/test_newprotocol.cpp"
        "src/newprotocol copy/tests/test_OT.cpp"
        "src/newprotocol copy/tests/test_Packed_PRF.cpp"
        "src/newprotocol copy/tests/test_packedMod2.cpp"
        "src/newprotocol copy/tests/test_packedMod3.cpp"
        "src/newprotocol copy/tests/test_PRF.cpp"
        "src/newprotocol copy/tests/test_Toeplitz-by-x.cpp"
        "src/newprotocol copy/tests/test_unit_newprotocol.cpp"
        "src/newprotocol copy/tests/test_unpacked_PRF.cpp"
        src/newprotocolprev/include/darkmatter/mains.hpp
        src/newprotocolprev/include/darkmatter/newprotocol.h
        src/newprotocolprev/include/darkmatter/OT.hpp
        src/newprotocolprev/include/darkmatter/packed_PRF_central.h
        src/newprotocolprev/include/darkmatter/packedMod2.hpp
        src/newprotocolprev/include/darkmatter/packedMod3.hpp
        src/newprotocolprev/include/darkmatter/PRF.hpp
        src/newprotocolprev/include/darkmatter/Timing.hpp
        src/newprotocolprev/include/darkmatter/Toeplitz-by-x.hpp
        src/newprotocolprev/include/darkmatter/unpacked_PRF_central.h
        src/newprotocolprev/include/darkmatter/utils.hpp
        src/newprotocolprev/src/newprotocol.cpp
        src/newprotocolprev/src/OT.cpp
        src/newprotocolprev/src/packed_PRF_central.cpp
        src/newprotocolprev/src/PRF.cpp
        src/newprotocolprev/src/Toeplitz-by-x.cpp
        src/newprotocolprev/src/unpacked_PRF_central.cpp
        src/newprotocolprev/tests/test_newprotocol.cpp
        src/newprotocolprev/tests/test_OT.cpp
        src/newprotocolprev/tests/test_Packed_PRF.cpp
        src/newprotocolprev/tests/test_packedMod2.cpp
        src/newprotocolprev/tests/test_packedMod3.cpp
        src/newprotocolprev/tests/test_PRF.cpp
        src/newprotocolprev/tests/test_Toeplitz-by-x.cpp
        src/newprotocolprev/tests/test_unpacked_PRF.cpp
        src/newprotocolprev2/include/darkmatter/mains.hpp
        src/newprotocolprev2/include/darkmatter/newprotocol.h
        src/newprotocolprev2/include/darkmatter/OT.hpp
        src/newprotocolprev2/include/darkmatter/packed_PRF_central.h
        src/newprotocolprev2/include/darkmatter/packedMod2.hpp
        src/newprotocolprev2/include/darkmatter/packedMod3.hpp
        src/newprotocolprev2/include/darkmatter/PRF.hpp
        src/newprotocolprev2/include/darkmatter/Timing.hpp
        src/newprotocolprev2/include/darkmatter/Toeplitz-by-x.hpp
        src/newprotocolprev2/include/darkmatter/unpacked_PRF_central.h
        src/newprotocolprev2/include/darkmatter/utils.hpp
        src/newprotocolprev2/src/newprotocol.cpp
        src/newprotocolprev2/src/OT.cpp
        src/newprotocolprev2/src/packed_PRF_central.cpp
        src/newprotocolprev2/src/PRF.cpp
        src/newprotocolprev2/src/Toeplitz-by-x.cpp
        src/newprotocolprev2/src/unpacked_PRF_central.cpp
        src/newprotocolprev2/tests/test_newprotocol.cpp
        src/newprotocolprev2/tests/test_OT.cpp
        src/newprotocolprev2/tests/test_Packed_PRF.cpp
        src/newprotocolprev2/tests/test_packedMod2.cpp
        src/newprotocolprev2/tests/test_packedMod3.cpp
        src/newprotocolprev2/tests/test_PRF.cpp
        src/newprotocolprev2/tests/test_Toeplitz-by-x.cpp
        src/newprotocolprev2/tests/test_unpacked_PRF.cpp
        src/newprotocoltemp/include/darkmatter/mains.hpp
        src/newprotocoltemp/include/darkmatter/newprotocol.h
        src/newprotocoltemp/include/darkmatter/OT.hpp
        src/newprotocoltemp/include/darkmatter/packed_PRF_central.h
        src/newprotocoltemp/include/darkmatter/packedMod2.hpp
        src/newprotocoltemp/include/darkmatter/packedMod3.hpp
        src/newprotocoltemp/include/darkmatter/PRF.hpp
        src/newprotocoltemp/include/darkmatter/Timing.hpp
        src/newprotocoltemp/include/darkmatter/Toeplitz-by-x.hpp
        src/newprotocoltemp/include/darkmatter/unpacked_PRF_central.h
        src/newprotocoltemp/include/darkmatter/utils.hpp
        src/newprotocoltemp/src/newprotocol.cpp
        src/newprotocoltemp/src/OT.cpp
        src/newprotocoltemp/src/packed_PRF_central.cpp
        src/newprotocoltemp/src/PRF.cpp
        src/newprotocoltemp/src/Toeplitz-by-x.cpp
        src/newprotocoltemp/src/unpacked_PRF_central.cpp
        src/newprotocoltemp/tests/test_newprotocol.cpp
        src/newprotocoltemp/tests/test_OT.cpp
        src/newprotocoltemp/tests/test_Packed_PRF.cpp
        src/newprotocoltemp/tests/test_packedMod2.cpp
        src/newprotocoltemp/tests/test_packedMod3.cpp
        src/newprotocoltemp/tests/test_PRF.cpp
        src/newprotocoltemp/tests/test_Toeplitz-by-x.cpp
        src/newprotocoltemp/tests/test_unpacked_PRF.cpp
        "src/old/2PC copy/comm/client.cpp"
        "src/old/2PC copy/comm/recv.cpp"
        "src/old/2PC copy/comm/recv.h"
        "src/old/2PC copy/comm/server.cpp"
        "src/old/2PC copy/comm/snd.cpp"
        "src/old/2PC copy/comm/snd.h"
        "src/old/2PC copy/include/darkmatter/data_receiver.h"
        "src/old/2PC copy/include/darkmatter/data_sender.h"
        "src/old/2PC copy/include/darkmatter/mains.hpp"
        "src/old/2PC copy/include/darkmatter/OT.hpp"
        "src/old/2PC copy/include/darkmatter/packedMod2.hpp"
        "src/old/2PC copy/include/darkmatter/packedMod3.hpp"
        "src/old/2PC copy/include/darkmatter/Toeplitz-by-x.hpp"
        "src/old/2PC copy/include/darkmatter/utils.hpp"
        "src/old/2PC copy/src/data_receiver.cpp"
        "src/old/2PC copy/src/data_sender.cpp"
        "src/old/2PC copy/src/OT.cpp"
        "src/old/2PC copy/src/Toeplitz-by-x.cpp"
        "src/old/2PC copy/src/Toeplitz-by-x_p1.cpp"
        "src/old/2PC copy/src/Toeplitz-by-x_p2.cpp"
        "src/old/2PC copy/tests/test_OT.cpp"
        "src/old/2PC copy/tests/test_packedMod2.cpp"
        "src/old/2PC copy/tests/test_packedMod3.cpp"
        "src/old/2PC copy/tests/test_Toeplitz-by-x.cpp"
        "src/old/darkmattercode copy/include/darkmatter/mains.hpp"
        "src/old/darkmattercode copy/include/darkmatter/OT.hpp"
        "src/old/darkmattercode copy/include/darkmatter/packedMod2.hpp"
        "src/old/darkmattercode copy/include/darkmatter/packedMod3.hpp"
        "src/old/darkmattercode copy/include/darkmatter/PRF.hpp"
        "src/old/darkmattercode copy/include/darkmatter/Timing.hpp"
        "src/old/darkmattercode copy/include/darkmatter/Toeplitz-by-x.hpp"
        "src/old/darkmattercode copy/include/darkmatter/utils.hpp"
        "src/old/darkmattercode copy/src/OT.cpp"
        "src/old/darkmattercode copy/src/PRF.cpp"
        "src/old/darkmattercode copy/src/Toeplitz-by-x.cpp"
        "src/old/darkmattercode copy/tests/test_OT.cpp"
        "src/old/darkmattercode copy/tests/test_packedMod2.cpp"
        "src/old/darkmattercode copy/tests/test_packedMod3.cpp"
        "src/old/darkmattercode copy/tests/test_PRF.cpp"
        "src/old/darkmattercode copy/tests/test_Toeplitz-by-x.cpp"
        "src/old/darkmattercode copy 2/include/darkmatter/mains.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/OT.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/packedMod2.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/packedMod3.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/PRF.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/Timing.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/Toeplitz-by-x.hpp"
        "src/old/darkmattercode copy 2/include/darkmatter/utils.hpp"
        "src/old/darkmattercode copy 2/src/OT.cpp"
        "src/old/darkmattercode copy 2/src/PRF.cpp"
        "src/old/darkmattercode copy 2/src/Toeplitz-by-x.cpp"
        "src/old/darkmattercode copy 2/tests/test_OT.cpp"
        "src/old/darkmattercode copy 2/tests/test_packedMod2.cpp"
        "src/old/darkmattercode copy 2/tests/test_packedMod3.cpp"
        "src/old/darkmattercode copy 2/tests/test_PRF.cpp"
        "src/old/darkmattercode copy 2/tests/test_Toeplitz-by-x.cpp"
        "src/old/secretsharing copy/dmweakPRF.cpp"
        "src/old/secretsharing copy/dmweakPRF.h"
        "src/old/secretsharing copy/dmweakPRFpacked.cpp"
        "src/old/secretsharing copy/dmweakPRFpacked.h"
        "src/old/secretsharing copy/mains.h"
        "src/old/secretsharing copy/pi23prot.cpp"
        "src/old/secretsharing copy/pi23prot.h"
        "src/old/secretsharing copy/pi23protpacked.cpp"
        "src/old/secretsharing copy/pi23protpacked.h"
        "src/old/secretsharing copy/pi_unit_test.cpp"
        "src/old/secretsharing copy/pi_unit_test.h"
        "src/old/secretsharing copy/piprot_test.cpp"
        "src/old/secretsharing copy/secShareTiming.cpp"
        "src/old/secretsharing copy/secShareTiming.h"
        "src/old/secretsharing copy/secShareTimingpacked.cpp"
        "src/old/secretsharing copy/secShareTimingpacked.h"
        "src/old/secretsharing copy/utils.cpp"
        "src/old/secretsharing copy/utils.h"
        "src/old/secretsharing copy/utilspacked.cpp"
        "src/old/secretsharing copy/utilspacked.h"
        src/PseudoRandomFunction/archivedcode/client_server_chat/client.cpp
        src/PseudoRandomFunction/archivedcode/client_server_chat/server.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/mains.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/OT.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/packedMod2.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/packedMod3.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/PRF.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/Timing.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/Toeplitz-by-x.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/include/darkmatter/utils.hpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/src/OT.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/src/PRF.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/src/Toeplitz-by-x.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/tests/test_OT.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/tests/test_packedMod2.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/tests/test_packedMod3.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/tests/test_PRF.cpp
        src/PseudoRandomFunction/archivedcode/darkmatter_time/tests/test_Toeplitz-by-x.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/mains.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/OT.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/packed_PRF_central.h
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/packedMod2.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/packedMod3.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/PRF.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/Timing.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/Toeplitz-by-x.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/unpacked_PRF_central.h
        src/PseudoRandomFunction/archivedcode/darkmattercode/include/darkmatter/utils.hpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/src/OT.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/src/packed_PRF_central.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/src/PRF.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/src/Toeplitz-by-x.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/src/unpacked_PRF_central.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_OT.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_Packed_PRF.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_packedMod2.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_packedMod3.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_PRF.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_Toeplitz-by-x.cpp
        src/PseudoRandomFunction/archivedcode/darkmattercode/tests/test_unpacked_PRF.cpp
        src/PseudoRandomFunction/archivedcode/naive/matrixvecmul.cpp
        src/PseudoRandomFunction/archivedcode/naive/matvecTiming.cpp
        src/PseudoRandomFunction/archivedcode/naive/optmatvec.cpp
        src/PseudoRandomFunction/archivedcode/naive/p1_test.cpp
        src/PseudoRandomFunction/archivedcode/naive/pack_unpack.cpp
        src/PseudoRandomFunction/archivedcode/naive/phase3.cpp
        src/PseudoRandomFunction/archivedcode/naive/phase3_grg.cpp
        src/PseudoRandomFunction/archivedcode/naive/pi_protocol.cpp
        src/PseudoRandomFunction/archivedcode/naive/prf.cpp
        src/PseudoRandomFunction/archivedcode/naive/PRF_naive.cpp
        src/PseudoRandomFunction/archivedcode/naive/PRF_test.cpp
        src/PseudoRandomFunction/archivedcode/naive/unit_edit.cpp
        src/PseudoRandomFunction/archivedcode/old/piprotocol/pi_protocol.cpp
        "src/PseudoRandomFunction/archivedcode/old/testing_old/unit_test copy.cpp"
        src/PseudoRandomFunction/archivedcode/old/testing_old/unit_test.cpp
        src/PseudoRandomFunction/archivedcode/old/testing_old/weak23prf.cpp
        src/PseudoRandomFunction/archivedcode/old/testing_old/weak23prf.h
        src/PseudoRandomFunction/archivedcode/old/matrixvecmul.cpp
        src/PseudoRandomFunction/archivedcode/old/matvecTiming.cpp
        src/PseudoRandomFunction/archivedcode/old/pi_protocol.cpp
        src/PseudoRandomFunction/archivedcode/old/prf.cpp
        src/PseudoRandomFunction/archivedcode/mat_vec_toeplitz.cpp
        src/PseudoRandomFunction/archivedcode/matrixvecmul.cpp
        src/PseudoRandomFunction/archivedcode/matvecTiming.cpp
        src/PseudoRandomFunction/archivedcode/pi_protocol.cpp
        src/PseudoRandomFunction/archivedcode/prf.cpp
        src/PseudoRandomFunction/archivedcode/PRF_naive.cpp
        src/PseudoRandomFunction/archivedcode/prf_naive2.cpp
        src/PseudoRandomFunction/misc/test/test/test.h
        src/PseudoRandomFunction/misc/test/test/test.m
        src/PseudoRandomFunction/src/communication_example/client.cpp
        src/PseudoRandomFunction/src/communication_example/server.cpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/mains.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/OT.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/packed_PRF_central.h
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/packedMod2.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/packedMod3.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/PRF.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/Timing.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/Toeplitz-by-x.hpp
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/unpacked_PRF_central.h
        src/PseudoRandomFunction/src/darkmattercode/include/darkmatter/utils.hpp
        src/PseudoRandomFunction/src/darkmattercode/src/OT.cpp
        src/PseudoRandomFunction/src/darkmattercode/src/packed_PRF_central.cpp
        src/PseudoRandomFunction/src/darkmattercode/src/PRF.cpp
        src/PseudoRandomFunction/src/darkmattercode/src/Toeplitz-by-x.cpp
        src/PseudoRandomFunction/src/darkmattercode/src/unpacked_PRF_central.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_OT.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_Packed_PRF.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_packedMod2.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_packedMod3.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_PRF.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_Toeplitz-by-x.cpp
        src/PseudoRandomFunction/src/darkmattercode/tests/test_unpacked_PRF.cpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/mains.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/newprotocol.h
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/OT.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/packed_PRF_central.h
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/packedMod2.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/packedMod3.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/PRF.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/Timing.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/Toeplitz-by-x.hpp
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/unpacked_PRF_central.h
        src/PseudoRandomFunction/src/newprotocol/include/darkmatter/utils.hpp
        src/PseudoRandomFunction/src/newprotocol/src/newprotocol.cpp
        src/PseudoRandomFunction/src/newprotocol/src/OT.cpp
        src/PseudoRandomFunction/src/newprotocol/src/packed_PRF_central.cpp
        src/PseudoRandomFunction/src/newprotocol/src/PRF.cpp
        src/PseudoRandomFunction/src/newprotocol/src/Toeplitz-by-x.cpp
        src/PseudoRandomFunction/src/newprotocol/src/unpacked_PRF_central.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_newprotocol.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_OT.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_Packed_PRF.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_packedMod2.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_packedMod3.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_PRF.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_Toeplitz-by-x.cpp
        src/PseudoRandomFunction/src/newprotocol/tests/test_unpacked_PRF.cpp
        src/PseudoRandomFunction/src/scunittest/dmweakPRFpacked.cpp
        src/PseudoRandomFunction/src/scunittest/dmweakPRFpacked.h
        src/PseudoRandomFunction/src/scunittest/pi23protpacked.cpp
        src/PseudoRandomFunction/src/scunittest/pi23protpacked.h
        src/PseudoRandomFunction/src/scunittest/pi_unit_test.cpp
        src/PseudoRandomFunction/src/scunittest/pi_unit_test.h
        src/PseudoRandomFunction/src/scunittest/piprot_test.cpp
        src/PseudoRandomFunction/src/scunittest/utils.cpp
        src/PseudoRandomFunction/src/scunittest/utils.h
        src/PseudoRandomFunction/src/secretsharing/dmweakPRF.cpp
        src/PseudoRandomFunction/src/secretsharing/dmweakPRF.h
        src/PseudoRandomFunction/src/secretsharing/dmweakPRFpacked.cpp
        src/PseudoRandomFunction/src/secretsharing/dmweakPRFpacked.h
        src/PseudoRandomFunction/src/secretsharing/mains.h
        src/PseudoRandomFunction/src/secretsharing/pi23prot.cpp
        src/PseudoRandomFunction/src/secretsharing/pi23prot.h
        src/PseudoRandomFunction/src/secretsharing/pi23protpacked.cpp
        src/PseudoRandomFunction/src/secretsharing/pi23protpacked.h
        src/PseudoRandomFunction/src/secretsharing/pi_unit_test.cpp
        src/PseudoRandomFunction/src/secretsharing/pi_unit_test.h
        src/PseudoRandomFunction/src/secretsharing/piprot_test.cpp
        src/PseudoRandomFunction/src/secretsharing/secShareTiming.cpp
        src/PseudoRandomFunction/src/secretsharing/secShareTiming.h
        src/PseudoRandomFunction/src/secretsharing/secShareTimingpacked.cpp
        src/PseudoRandomFunction/src/secretsharing/secShareTimingpacked.h
        src/PseudoRandomFunction/src/secretsharing/utils.cpp
        src/PseudoRandomFunction/src/secretsharing/utils.h
        src/PseudoRandomFunction/src/secretsharing/utilspacked.cpp
        src/PseudoRandomFunction/src/secretsharing/utilspacked.h
        src/PseudoRandomFunction/src/testing/unit_test.cpp
        src/PseudoRandomFunction/src/testing/weakprf.cpp
        src/PseudoRandomFunction/src/testing/weakprf.h
        src/PseudoRandomFunction/src/two_party_comm2/comm/client.cpp
        src/PseudoRandomFunction/src/two_party_comm2/comm/recv.cpp
        src/PseudoRandomFunction/src/two_party_comm2/comm/recv.h
        src/PseudoRandomFunction/src/two_party_comm2/comm/server.cpp
        src/PseudoRandomFunction/src/two_party_comm2/comm/snd.cpp
        src/PseudoRandomFunction/src/two_party_comm2/comm/snd.h
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/data_receiver.h
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/data_sender.h
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/OT.hpp
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/packedMod2.hpp
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/packedMod3.hpp
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/Toeplitz-by-x.hpp
        src/PseudoRandomFunction/src/two_party_comm2/include/darkmatter/utils.hpp
        src/PseudoRandomFunction/src/two_party_comm2/src/data_receiver.cpp
        src/PseudoRandomFunction/src/two_party_comm2/src/data_sender.cpp
        src/PseudoRandomFunction/src/two_party_comm2/src/OT.cpp
        src/PseudoRandomFunction/src/two_party_comm2/src/Toeplitz-by-x.cpp
        src/PseudoRandomFunction/src/two_party_comm2/src/Toeplitz-by-x_p1.cpp
        src/PseudoRandomFunction/src/two_party_comm2/src/Toeplitz-by-x_p2.cpp
        src/PseudoRandomFunction/src/two_party_comm2/tests/test_OT.cpp
        src/PseudoRandomFunction/src/two_party_comm2/tests/test_packedMod2.cpp
        src/PseudoRandomFunction/src/two_party_comm2/tests/test_packedMod3.cpp
        src/PseudoRandomFunction/src/two_party_comm2/tests/test_Toeplitz-by-x.cpp
        src/PseudoRandomFunction/src/unitTesting/dmweakPRF.cpp
        src/PseudoRandomFunction/src/unitTesting/dmweakPRF.h
        src/PseudoRandomFunction/src/unitTesting/prfTiming.cpp
        src/PseudoRandomFunction/src/unitTesting/unittestPRF.cpp
        src/PseudoRandomFunction/src/unitTesting/weakPRFtest.cpp
        src/PseudoRandomFunction/src/unitTesting/weakPRFtest.h
        src/PseudoRandomFunction/src/wordpacked/prfTiming.cpp
        src/PseudoRandomFunction/matrixvecmul.cpp
        src/PseudoRandomFunction/PRF_naive.cpp
        src/scunittest/dmweakPRFpacked.cpp
        src/scunittest/dmweakPRFpacked.h
        src/scunittest/pi23protpacked.cpp
        src/scunittest/pi23protpacked.h
        src/scunittest/pi_unit_test.cpp
        src/scunittest/pi_unit_test.h
        src/scunittest/piprot_test.cpp
        src/scunittest/utils.cpp
        src/scunittest/utils.h
        src/secretsharing/dmweakPRF.cpp
        src/secretsharing/dmweakPRF.h
        src/secretsharing/dmweakPRFpacked.cpp
        src/secretsharing/dmweakPRFpacked.h
        src/secretsharing/mains.h
        src/secretsharing/pi23prot.cpp
        src/secretsharing/pi23prot.h
        src/secretsharing/pi23protpacked.cpp
        src/secretsharing/pi23protpacked.h
        src/secretsharing/pi_unit_test.cpp
        src/secretsharing/pi_unit_test.h
        src/secretsharing/piprot_test.cpp
        src/secretsharing/secShareTiming.cpp
        src/secretsharing/secShareTiming.h
        src/secretsharing/secShareTimingpacked.cpp
        src/secretsharing/secShareTimingpacked.h
        src/secretsharing/utils.cpp
        src/secretsharing/utils.h
        src/secretsharing/utilspacked.cpp
        src/secretsharing/utilspacked.h
        src/testing/unit_test.cpp
        src/testing/weakprf.cpp
        src/testing/weakprf.h
        src/two_party_comm2/comm/client.cpp
        src/two_party_comm2/comm/recv.cpp
        src/two_party_comm2/comm/recv.h
        src/two_party_comm2/comm/server.cpp
        src/two_party_comm2/comm/snd.cpp
        src/two_party_comm2/comm/snd.h
        src/two_party_comm2/include/darkmatter/data_receiver.h
        src/two_party_comm2/include/darkmatter/data_sender.h
        src/two_party_comm2/include/darkmatter/OT.hpp
        src/two_party_comm2/include/darkmatter/packedMod2.hpp
        src/two_party_comm2/include/darkmatter/packedMod3.hpp
        src/two_party_comm2/include/darkmatter/Toeplitz-by-x.hpp
        src/two_party_comm2/include/darkmatter/utils.hpp
        src/two_party_comm2/src/data_receiver.cpp
        src/two_party_comm2/src/data_sender.cpp
        src/two_party_comm2/src/OT.cpp
        src/two_party_comm2/src/Toeplitz-by-x.cpp
        src/two_party_comm2/src/Toeplitz-by-x_p1.cpp
        src/two_party_comm2/src/Toeplitz-by-x_p2.cpp
        src/two_party_comm2/tests/test_OT.cpp
        src/two_party_comm2/tests/test_packedMod2.cpp
        src/two_party_comm2/tests/test_packedMod3.cpp
        src/two_party_comm2/tests/test_Toeplitz-by-x.cpp
        src/unitTesting/dmweakPRF.cpp
        src/unitTesting/dmweakPRF.h
        src/unitTesting/prfTiming.cpp
        src/unitTesting/unittestPRF.cpp
        src/unitTesting/weakPRFtest.cpp
        src/unitTesting/weakPRFtest.h
        src/wordpacked/prfTiming.cpp
        matrixvecmul.cpp
        PRF_naive.cpp)
