set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "ARM")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -march=armv7-a -marm -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=/opt/glory-x11/2.4.4/sysroots/cortexa9hf-neon-poky-linux-gnueabi ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -march=armv7-a -marm -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=/opt/glory-x11/2.4.4/sysroots/cortexa9hf-neon-poky-linux-gnueabi -fpermissive -Wno-write-strings -Wno-return-local-addr -Wno-pointer-arith -Wno-conversion-null -Wno-class-conversion -Wno-int-to-pointer-cast -g")
set(CMAKE_C_COMPILER "/opt/glory-x11/2.4.4/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "/opt/glory-x11/2.4.4/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++")

set(CMAKE_FIND_ROOT_PATH /opt/arm)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -msse2" CACHE STRING "c++ flags")
# set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32 -msse2" CACHE STRING "c flags")
# set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -m32 -msse2" CACHE STRING "asm flags")

set(CMAKE_PREFIX_PATH "/opt/arm/boost_1_71_0;/opt/arm/openssl-1.0.0g;/opt/arm/opencv-3.4.10")
set(Boost_USE_STATIC_LIBS ON)

add_compile_definitions(
    # DUMMY_CAP
)

link_directories(
    ${PROJECT_SOURCE_DIR}/build_arm
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_arm)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_arm)
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_arm)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_arm)

set(CMAKE_INSTALL_RPATH "./")
set(CMAKE_PROGRAM_PATH "./")
set(CMAKE_LIBRARY_PATH "./")
