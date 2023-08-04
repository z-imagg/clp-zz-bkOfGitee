#ref: https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling
#cmake -DCT_Clang_INSTALL_DIR=/pubx/build-llvm15/  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCURSES_LIBRARY=/lib64/libncurses.so.6 -DCURSES_INCLUDE_PATH=/usr/include/   -DCMAKE_EXPORT_COMPILE_COMMANDS=True   -DCMAKE_C_COMPILER=/pubx/build-llvm15/bin/clang -DCMAKE_CXX_COMPILER=/pubx/build-llvm15/bin/clang++    -DLLVM_DIR=/pubx/build-llvm15 ..

#cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake ..
SET(CMAKE_C_COMPILER   /pubx/build-llvm15/bin/clang)
SET(CMAKE_CXX_COMPILER /pubx/build-llvm15/bin/clang++)
