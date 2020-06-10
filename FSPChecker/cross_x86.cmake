set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CMAKE_SYSROOT /)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wno-write-strings -Wno-return-local-addr -Wno-pointer-arith -Wno-conversion-null -Wno-class-conversion -Wno-int-to-pointer-cast -g")
set(CMAKE_C_COMPILER /usr/bin/i686-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/i686-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /opt/x86)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -msse2" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32 -msse2" CACHE STRING "c flags")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -m32 -msse2" CACHE STRING "asm flags")

set(CMAKE_PREFIX_PATH "/opt/x86/boost_1_71_0;/opt/x86/openssl-1.0.0g;/opt/x86/opencv-3.4.10")
set(Boost_USE_STATIC_LIBS ON)

add_compile_definitions(
    # DUMMY_CAP
)

link_directories(
    ${PROJECT_SOURCE_DIR}/build
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

set(CMAKE_INSTALL_RPATH "./")
set(CMAKE_PROGRAM_PATH "./")
set(CMAKE_LIBRARY_PATH "./")
