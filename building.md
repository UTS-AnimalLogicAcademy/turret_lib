At present, Linux is the only supported build platform, however as the build system is CMake it should in theory be possible to build on other platforms.  

Requirements:
* libzmq-4.2.3
* cppzmq-4.3.0
* boost-1.55
* cmake-3.2

Other versions may work but are untested.  

This repository includes a modified FindZeroMQ.cmake module, which finds the libzmq headers and libraries, as well as setting a cmake variable CPPZMQ_INCLUDE_DIRS based on an environment variable $CPPZMQ_INCLUDE.  This locates cppzmq headers such as zmq.hpp.  Setting this environment variable is the easiest way to set everything up.  This repo also uses the cmake standard FindBoost.cmake module.  

At UTSALA we build and deploy packages using rez (https://github.com/nerdvegas/rez), which takes care of resolving package dependencies and building variants, and the build destination path:

rez-build -i

However, provided your environment is setup correctly, you should also be able to build via the standard cmake process:

mkdir build
cd build
cmake ..
make DESTDIR=/install/path install

or

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path ..
make install




