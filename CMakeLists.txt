cmake_minimum_required(VERSION 3.2)

set (CMAKE_CXX_STANDARD 11)

if(WIN32)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_SOURCE_DIR}/cmake/modules
        )

project(turret_lib)

add_library(turret SHARED
        src/turretLogger.cpp
        src/turretClient.cpp
        )

find_package(ZeroMQ REQUIRED)

if(MSVC)
        add_definitions( -DBOOST_ALL_NO_LIB )
endif(MSVC)

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

file(GLOB_RECURSE inc_files "include/*")
install(FILES ${inc_files} DESTINATION include)

target_link_libraries(turret
        ${ZeroMQ_LIBRARY}
        ${Boost_LIBRARIES}
        )
