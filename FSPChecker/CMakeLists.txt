cmake_minimum_required (VERSION 3.12.0)

project(FSPChecker)

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


if(Boost_FOUND)
    message(STATUS "Found Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")
    message(STATUS "Found Boost_LIBRARIES ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(
    Compat
)

# set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)


add_compile_definitions(
    # DEBUG_MUTEX_LOG
    # USE_RELATIVE_PATH
    # WINXP
    DEBUG
)
add_subdirectory(Compat)
add_subdirectory(FSPCheckerCpp)
