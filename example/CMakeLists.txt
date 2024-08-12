cmake_minimum_required(VERSION 3.16)
set(C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -Wall -Wextra \
                            -Wno-unused-function \
                            -Wno-unused-result \
                            -Wno-unused-parameter \
                            -O0 -g -fPIC \
                            -fstack-protector-all")

    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -Wextra -g -ggdb \
                              -Wno-unused-label -Wno-unused-variable \
                              -Wno-unused-function \
                              -Wno-unused-result \
                              -Wno-unused-parameter \
                              -Wno-char-subscripts \
                              -fstack-protector-all")
    message(STATUS "CMAKE_BUILD_TYPE = Debug")
else()
    set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -Wall -Wextra \
                                -Wno-unused-function \
                                -Wno-unused-result \
                                -Wno-unused-parameter \
                                -O3 -s -fPIC \
                                -fstack-protector-all")
                                
    set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -Wextra -fPIC \
                                -Wno-unused-label -Wno-unused-variable \
                                -Wno-unused-function \
                                -Wno-unused-result \
                                -Wno-unused-parameter \
                                -Wno-char-subscripts \
                                -fstack-protector-all")
    message(STATUS "CMAKE_BUILD_TYPE = Release")
endif()

set(TARGET "librtsp_test")

project(${TARGET})

message("ARCH is ${ARCH}")

include_directories(
    ${CMAKE_SOURCE_DIR}/3rdparty/librtsp/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/3rdparty/librtsp/lib
)

## src includes
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

aux_source_directory(${CMAKE_SOURCE_DIR}/src DIR_SRCS)

add_executable(
    ${TARGET}
    ${DIR_SRCS}
)

add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_STRIP} ${TARGET}
)

add_definitions(
    -DCMAKE_TOOLCHAIN_FILE
    -DSOCKLEN_T=socklen_t 
    -D_LARGEFILE_SOURCE=1
    -D_FILE_OFFSET_BITS=64
    -DOS_LINUX
    -DLOG_TO_SCREEN
    -DFGRELEASE
)

target_link_libraries(
    ${TARGET}
    pthread m dl rtsp
)