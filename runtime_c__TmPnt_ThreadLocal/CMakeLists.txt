
cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_C_STANDARD 11)

#Makefile繁琐模式
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

#项目名称
project(clangPlugin-Var--runtime_c__TmPnt_ThreadLocal)


include("${CMAKE_CURRENT_SOURCE_DIR}/../ClAsCxx.cmake")

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# libclangPlgVar_runtime_cTmPntThrLcl.a 开始
add_library(clangPlgVar_runtime_cTmPntThrLcl STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
        )

target_include_directories(clangPlgVar_runtime_cTmPntThrLcl
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
        )
# libclangPlgVar_runtime_cTmPntThrLcl.a 结束

# _c__test_main.elf 开始
set(_c__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_C.c"
)
add_executable(
_c__test_main.elf
  ${_c__SOURCES}
)
target_include_directories(
_c__test_main.elf
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)
# _c__test_main.elf 结束

# _cxx__test_main.elf 开始
set(_cxx__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_CXX.cpp"
)
add_executable(
_cxx__test_main.elf
  ${_cxx__SOURCES}
)
target_include_directories(
_cxx__test_main.elf
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)
# _cxx__test_main.elf 结束

# _c__test_main_linkStaticArch.elf 开始
# 示范 c如何 使用 libclangPlgVar_runtime_cTmPntThrLcl.a
set(_c_LinkStaticArch__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_C.c"
)
add_executable( _c__test_main_linkStaticArch.elf
${_c_LinkStaticArch__SOURCES}
)
target_include_directories( _c__test_main_linkStaticArch.elf
PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)
#指定链接静态库 libclangPlgVar_runtime_cTmPntThrLcl.a
target_link_libraries(_c__test_main_linkStaticArch.elf
PRIVATE
/fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/cmake-build-debug/libclangPlgVar_runtime_cTmPntThrLcl.a
)
# _c__test_main_linkStaticArch.elf 结束



# _cxx__test_main_linkStaticArch.elf 开始
# 示范 c++如何 使用 libclangPlgVar_runtime_cTmPntThrLcl.a
set(_cxx_LinkStaticArch__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_CXX.cpp"
)
add_executable(_cxx__test_main_linkStaticArch.elf
${_cxx_LinkStaticArch__SOURCES}
)
target_include_directories( _cxx__test_main_linkStaticArch.elf
PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)
# _cxx__test_main_linkStaticArch.elf 结束
