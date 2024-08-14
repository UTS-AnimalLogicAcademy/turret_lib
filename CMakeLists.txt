cmake_minimum_required(VERSION 3.2)

set (CMAKE_CXX_STANDARD 17)

if(DEFINED ENV{CXX11_ABI_IGNORE})
        message(INFO " CXX11_ABI_IGNORE set. Ignoring CXX11 flags.")
        set(CMAKE_CXX_FLAGS "${_PXR_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 ${CMAKE_CXX_FLAGS}")
endif(DEFINED ENV{CXX11_ABI_IGNORE})

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

if(WIN32)
        add_definitions( -DBOOST_ALL_NO_LIB )
endif(WIN32)

set(BOOST_LIBRARY_DIR $ENV{BOOST_ROOT})

find_package(Boost
        COMPONENTS
        filesystem
        serialization
        REQUIRED
        )

set(TBB_ROOT_DIR $ENV{TBBROOT})
set(TBB_INCLUDE_DIR $ENV{TBB_INCLUDE_DIR})
set(TBB_LIBRARY $ENV{TBB_LIBRARIES})

find_package(TBB REQUIRED COMPONENTS tbb)
add_definitions(${TBB_DEFINITIONS})

target_include_directories(turret PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PC_LIBZMQ_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CPPZMQ_INCLUDE_DIRS}
        ${TBB_INCLUDE_DIR}
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
        ${TBB_LIBRARIES}
        )
