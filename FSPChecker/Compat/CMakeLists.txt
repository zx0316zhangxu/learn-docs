find_package(
    Boost
    1.71.0
    REQUIRED
    COMPONENTS
    date_time
    filesystem
    iostreams
    system
    thread
    log
)

# find_package(
#     Iconv
#     REQUIRED
# )

# set(Boost_FOUND true)
# set(Boost_INCLUDE_DIRS "/mnt/m/Develop/UWF_Linux/boost_1_71_0")
# set(
#     Boost_LIBRARIES
#     "/mnt/m/Develop/UWF_Linux/boost_1_71_0/stage/lib/libboost_date_time.so"
#     "/mnt/m/Develop/UWF_Linux/boost_1_71_0/stage/lib/libboost_filesystem.so"
#     "/mnt/m/Develop/UWF_Linux/boost_1_71_0/stage/lib/libboost_iostreams.so"
#     "/mnt/m/Develop/UWF_Linux/boost_1_71_0/stage/lib/libboost_system.so"
#     "/mnt/m/Develop/UWF_Linux/boost_1_71_0/stage/lib/libboost_thread.so"
#     "/mnt/m/Develop/UWF_Linux/boost_1_71_0/stage/lib/libboost_log.so"
# )
# set(Iconv_FOUND true)
# set(Iconv_INCLUDE_DIRS "/usr/local/include")
# set(
#     Iconv_LIBRARIES
#     "/usr/local/lib/libiconv.so"
# )

# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -init=_wrapper_init -fini=_wrapper_fini")

if(Boost_FOUND)
    message(STATUS "Found Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")
    message(STATUS "Found Boost_LIBRARIES ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# if(Iconv_FOUND)
#     message(STATUS "Found Iconv_INCLUDE_DIRS ${Iconv_INCLUDE_DIRS}")
#     message(STATUS "Found Iconv_LIBRARIES ${Iconv_LIBRARIES}")
#     include_directories(${Iconv_INCLUDE_DIRS})
# endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fpermissive")
else()
     message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()
include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=c11" COMPILER_SUPPORTS_C11)
if(COMPILER_SUPPORTS_C11)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pthread")
        add_compile_definitions(
            C11_AVAILIABLE
        )
else()
     message(STATUS "The compiler ${CMAKE_C_COMPILER} has no C11 support. Please use a different C compiler.")
endif()

include_directories(
    .
    ./safestringlib/include
    ./safestringlib/safeclib
)

add_compile_options(
    -fPIC
)

add_library(
    Compat
    STATIC
    
    common_handle.cpp

    fileapi.cpp
    libloaderapi.cpp
    profileapi.cpp
    synchapi.cpp
    sysinfoapi.cpp
    tchar.c
    tchar.cpp
    timezoneapi.cpp
    winbase.cpp
    stdio_win.cpp
    stdlib_win.cpp
    debugapi.cpp
    handleapi.cpp
    ioapi.cpp
    ioapiset.cpp
    rpcdce.cpp
    errhandlingapi.cpp
    processthreadsapi.cpp
    process.cpp
    io.cpp
    direct.cpp
    shellapi.cpp
    winreg.cpp
    trace.cpp
    eventapi.cpp
    winuser.cpp
    wingdi.cpp
    winsock2.cpp
    ./safestringlib/safeclib/strncat_s.c
    ./safestringlib/safeclib/strncpy_s.c
    ./safestringlib/safeclib/strnlen_s.c
    ./safestringlib/safeclib/strcpy_s.c
    ./safestringlib/safeclib/strcat_s.c
    ./safestringlib/safeclib/safe_mem_constraint.c
    ./safestringlib/safeclib/safe_str_constraint.c
    ./safestringlib/safeclib/ignore_handler_s.c
)

target_link_libraries(
    Compat
    ${Boost_LIBRARIES}
    ${Iconv_LIBRARIES}
)
