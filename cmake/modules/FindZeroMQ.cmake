if(UNIX)
    set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(PC_LIBZMQ QUIET libzmq)
    endif()
endif()

# Prefer Rez package root if present
find_path(ZeroMQ_INCLUDE_DIR
    NAMES zmq.h
    PATHS
        $ENV{REZ_LIBZMQ_ROOT}/include
        ${PC_LIBZMQ_INCLUDEDIR}
        ${PC_LIBZMQ_INCLUDE_DIRS}
)

find_library(ZeroMQ_LIBRARY
    NAMES zmq libzmq libzmq.so libzmq.dylib libzmq.dll
    PATHS
        $ENV{REZ_LIBZMQ_ROOT}/lib
        $ENV{REZ_LIBZMQ_ROOT}/lib64
        ${PC_LIBZMQ_LIBDIR}
        ${PC_LIBZMQ_LIBRARY_DIRS}
)

find_library(ZeroMQ_STATIC_LIBRARY
    NAMES zmq-static libzmq-static libzmq-static.a libzmq.a libzmq.lib
    PATHS
        $ENV{REZ_LIBZMQ_ROOT}/lib
        $ENV{REZ_LIBZMQ_ROOT}/lib64
        ${PC_LIBZMQ_LIBDIR}
        ${PC_LIBZMQ_LIBRARY_DIRS}
)

set(CPPZMQ_INCLUDE_DIRS $ENV{CPPZMQ_INCLUDE})
set(ZeroMQ_VERSION ${PC_LIBZMQ_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZeroMQ
    REQUIRED_VARS ZeroMQ_LIBRARY ZeroMQ_INCLUDE_DIR
    VERSION_VAR ZeroMQ_VERSION
)

if(TARGET libzmq)
    return()
endif()

add_library(libzmq SHARED IMPORTED)
set_property(TARGET libzmq PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ZeroMQ_INCLUDE_DIR}")
set_property(TARGET libzmq PROPERTY IMPORTED_LOCATION "${ZeroMQ_LIBRARY}")

if(ZeroMQ_STATIC_LIBRARY)
    add_library(libzmq-static STATIC IMPORTED)
    set_property(TARGET libzmq-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ZeroMQ_INCLUDE_DIR}")
    set_property(TARGET libzmq-static PROPERTY IMPORTED_LOCATION "${ZeroMQ_STATIC_LIBRARY}")
endif()