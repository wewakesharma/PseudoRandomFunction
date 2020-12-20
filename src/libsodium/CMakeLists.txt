cmake_minimum_required(VERSION 3.16)
project(libsodium)

set(CMAKE_CXX_STANDARD 14)

add_executable(libsodium main.cpp)

include_directories(/usr/local/include;/usr/local/include/sodium)

set(LD_LIBRARY_PATH=/usr/local/lib )

set(LDFLAGS=$(pkg-config --libs libsodium))
set(CFLAGS=$(pkg-config --cflags libsodium))

set(LINK_DIR /usr/local/lib)
set (-l sodium)

TARGET_LINK_LIBRARIES(libsodium)