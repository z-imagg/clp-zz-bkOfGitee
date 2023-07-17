cmake_minimum_required(VERSION 3.13.4)

#===============================================================================
# 0. GET CLANG INSTALLATION DIR
#修改默认编译器
set(CT_Clang_INSTALL_DIR "/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CURSES_LIBRARY "/lib64/libncurses.so.6")
set(CURSES_INCLUDE_PATH "/usr/include/")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
#编译器还是使用自带的gcc, 否则 调试时 没有 libstdc++  的调试信息，导致std::string在gdb中不显示，参考：https://stackoverflow.com/questions/58356385/python-exception-class-gdb-error-there-is-no-member-named-m-dataplus-whe/58356946#58356946
#   gdb显示std::string时报错: There is no member named _M_dataplus。 因此gdb不显示std::string的值.
#set(CMAKE_C_COMPILER "/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang")
#set(CMAKE_CXX_COMPILER "/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++")
set(LLVM_DIR "/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4")
#set(xxx "")

project(clang-tutor)
#project放到默认编译器定义之后，否则cmake会死循环


set(CT_LLVM_INCLUDE_DIR "${CT_Clang_INSTALL_DIR}/include/llvm")

set(CT_LLVM_CMAKE_FILE "${CT_Clang_INSTALL_DIR}/lib/cmake/clang/ClangConfig.cmake")

# http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
list(APPEND CMAKE_PREFIX_PATH "${CT_Clang_INSTALL_DIR}/lib/cmake/clang/")

find_package(Clang REQUIRED CONFIG)

# Sanity check. As Clang does not expose e.g. `CLANG_VERSION_MAJOR` through
# AddClang.cmake, we have to use LLVM_VERSION_MAJOR instead.
# TODO: Revisit when next version is released.
if(NOT "15" VERSION_EQUAL "${LLVM_VERSION_MAJOR}")
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 15")
endif()

message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using ClangConfig.cmake in: ${CT_Clang_INSTALL_DIR}")

message("CLANG STATUS:
  Includes (clang)    ${CLANG_INCLUDE_DIRS}
  Includes (llvm)     ${LLVM_INCLUDE_DIRS}"
)

# Set the LLVM and Clang header and library paths
include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")

#===============================================================================
# 3. CLANG-TUTOR BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
    -fdiagnostics-color=always")

# LLVM/Clang is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden"
  SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if(${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#===============================================================================
# 4. ADD SUB-TARGETS
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#===============================================================================
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(HelloWorld)
