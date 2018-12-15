cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_STANDARD 11)

include(RezBuild)

project(zmq_client_cpp)



add_library(zmq_client_cpp SHARED
    src/zmqLogger.cpp
    src/zmqClient.cpp
)


# boost
macro(set_option option value) 
    set(${option} ${value} CACHE INTERNAL ""FORCE)
endmacro()

set_option(BOOST_ROOT $ENV{REZ_BOOST_ROOT})

target_include_directories(zmq_client_cpp PRIVATE include/zmq_client_cpp)
target_include_directories(zmq_client_cpp PRIVATE $ENV{LIBZMQ_INCLUDE} $ENV{BOOST_ROOT}/include)

target_include_directories(zmq_client_cpp PRIVATE lib/)

install(TARGETS zmq_client_cpp DESTINATION lib/)

target_link_libraries(zmq_client_cpp $ENV{LIBZMQ_LIB}libzmq.so $ENV{BOOST_ROOT}/lib/libboost_serialization.so $ENV{BOOST_ROOT}/lib/libboost_filesystem.so)

file(GLOB_RECURSE inc_files "include/*")
rez_install_files(
    ${inc_files}
    DESTINATION .
    EXECUTABLE
    LOCAL_SYMLINK
)
