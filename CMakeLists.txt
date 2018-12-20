cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        "/mnt/ala/software/pipeline/cmake/modules/rez"
        ${CMAKE_SOURCE_DIR}/cmake/modules
        )

include(RezBuild)

project(turret_lib)

add_library(turret SHARED
        src/turretLogger.cpp
        src/turretClient.cpp
        )

find_package(ZeroMQ REQUIRED)

find_package(Boost
        COMPONENTS
        filesystem
        serialization
        REQUIRED
        )

target_include_directories(turret PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PC_LIBZMQ_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CPPZMQ_INCLUDE_DIRS}
        )

install(TARGETS turret DESTINATION lib/)

target_link_libraries(turret
        ${ZeroMQ_LIBRARY}
        ${Boost_LIBRARIES}
        )

file(GLOB_RECURSE inc_files "include/*")
rez_install_files(
        ${inc_files}
        DESTINATION .
        EXECUTABLE
        LOCAL_SYMLINK
)
