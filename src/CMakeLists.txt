cmake_minimum_required(VERSION 3.12)
project(dscat)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DOPENSSL_COMPATIBLE_10) # link Openssl v1.0.x

include_directories(/usr/local/opt/openssl/include)
link_directories(/usr/local/opt/openssl/lib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /usr/local/opt/openssl/lib/libssl.a /usr/local/opt/openssl/lib/libcrypto.a ${OPT_LDFLAGS}")

add_executable( dscat main.cpp lib/scatlib.hpp lib/base64.hpp lib/clipp.h lib/cuilog.hpp lib/colorstreams.hpp lib/hash.hpp)
