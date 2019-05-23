@echo off

rd /s /q build

mkdir build
cd build

SET CPPZMQ_INCLUDE=C:/Software/cppzmq
SET LIBZMQ_INCLUDE_DIR=C:/Software/libzmq/include
SET LIBZMQ_LIB_DIR=C:/Software/libzmq/build/lib/Release/libzmq-v140-mt-4_3_2.lib

cmake -DZeroMQ_LIBRARY:STRING=%LIBZMQ_LIB_DIR% -DPC_LIBZMQ_INCLUDE_DIRS:STRING=%LIBZMQ_INCLUDE_DIR% -DCMAKE_INSTALL_PREFIX="C:/Software/turret-lib" -G "Visual Studio 14 2015 Win64" ..
cmake --build . --config Release --target install -- /M:16

cd ..
