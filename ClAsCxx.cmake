#测试本脚本的命令: rm -fr ./build;  cmake -C ClAsCxx.cmake  -B ./build/ ; gedit $(ls /tmp/cmkVs__diff*)
#cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(LLVM_HOME "/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4")
list(APPEND CMAKE_MODULE_PATH "${LLVM_HOME}")

list(APPEND CMAKE_PREFIX_PATH "${LLVM_HOME}/lib/cmake/llvm/")
find_package(LLVM REQUIRED CONFIG)#产生了很多变量
#此句会 产生变量 LLVM_DIR
message(STATUS "LLVM_PACKAGE_VERSION=【${LLVM_PACKAGE_VERSION}】")
message(STATUS "LLVM_DIR=【${LLVM_DIR}】")

list(APPEND CMAKE_PREFIX_PATH "${LLVM_HOME}/lib/cmake/clang/")
find_package(Clang REQUIRED CONFIG)

message(STATUS "LLVM_CMAKE_DIR=【${LLVM_CMAKE_DIR}】")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM) #产生了 CMAKE_INSTALL_FULL_LIBDIR=/usr/local/lib

message("LLVM 有 :
  LLVM_DEFINITIONS=【${LLVM_DEFINITIONS}】
  LLVM_INCLUDE_DIRS=【${LLVM_INCLUDE_DIRS}】
  LLVM_LIBRARY_DIRS=【${LLVM_LIBRARY_DIRS}】
  LLVM_TARGETS_TO_BUILD=【${LLVM_TARGETS_TO_BUILD}】"
        )

include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")
# Now set the LLVM header and library paths:
#include_directories(${LLVM_INCLUDE_DIRS})
#link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

#set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
#    -fdiagnostics-color=always")
#include(CheckCXXCompilerFlag)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ")

########



#set(CMAKE_C_COMPILER "${LLVM_HOME}/bin/clang")
set(CMAKE_C_COMPILER "${LLVM_HOME}/bin/clang")
set(CMAKE_CXX_COMPILER "${LLVM_HOME}/bin/clang++")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
#set(CMAKE_CXX_LINK_EXECUTABLE "${LLVM_HOME}/bin/ld.lld" )#CMAKE_CXX_LINK_EXECUTABLE
#set(CMAKE_LINKER ${LLVM_HOME}/bin/lld)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=${LLVM_HOME}/bin/lld")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fuse-ld=${LLVM_HOME}/bin/ld.lld")
#set(CMAKE_EXE_LINKER_FLAGS "-L${LLVM_HOME}/bin -fuse-ld=ld.lld")





MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /bal/clang-add-funcIdAsm/tools




#target_link_libraries(nlohmann--json-test
#        INTERFACE
#        ${REQ_LLVM_LIBRARIES}
#        )

#set_target_properties(nlohmann--json-test PROPERTIES
#        LINKER_LANGUAGE CXX
#        CXX_STANDARD 17
#        )

#set_property(TARGET nlohmann--json-test PROPERTY CXX_STANDARD 17)
#set_property(TARGET nlohmann--json-test PROPERTY CXX_STANDARD_REQUIRED ON)


#    target_include_directories(
#    nlohmann--json-test
#      PRIVATE
#      "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
#        "${LIBFMT_INCLUDE}"
##            "${LLVM_HOME}/lib/clang/15.0.0/include"
#            #-isystem ${LLVM_HOME}/lib/clang/15.0.0/include
#    )


#MESSAGE(STATUS "nlohmann--json-test,,LIBFMT_STATIC:${LIBFMT_STATIC};LIBFMT_INCLUDE:${LIBFMT_INCLUDE}")
