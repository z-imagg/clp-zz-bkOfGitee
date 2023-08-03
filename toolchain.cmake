#ref: https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling
#cmake -DCT_Clang_INSTALL_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCURSES_LIBRARY=/lib64/libncurses.so.6 -DCURSES_INCLUDE_PATH=/usr/include/   -DCMAKE_EXPORT_COMPILE_COMMANDS=True   -DCMAKE_C_COMPILER=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -DCMAKE_CXX_COMPILER=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++    -DLLVM_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4 ..

#cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake ..
SET(CMAKE_C_COMPILER   /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang)
SET(CMAKE_CXX_COMPILER /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++)
