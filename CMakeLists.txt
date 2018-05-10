cmake_minimum_required(VERSION 2.6)

include(RezBuild)

project(zmq_client_cpp)


add_library(zmq_client_cpp SHARED
    src/zmqClient.cpp
)

target_include_directories(zmq_client_cpp PRIVATE include/zmq_client_cpp)
target_include_directories(zmq_client_cpp PRIVATE $ENV{LIBZMQ_INCLUDE})

target_include_directories(zmq_client_cpp PRIVATE lib/)

install(TARGETS zmq_client_cpp DESTINATION lib/)

target_link_libraries(zmq_client_cpp $ENV{LIBZMQ_LIB}libzmq.so)

file(GLOB_RECURSE inc_files "include/*")
rez_install_files(
    ${inc_files}
    DESTINATION .
    EXECUTABLE
    LOCAL_SYMLINK
)
