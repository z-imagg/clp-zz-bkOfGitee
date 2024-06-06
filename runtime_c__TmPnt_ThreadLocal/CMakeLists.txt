
cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_C_STANDARD 11)

#Makefile繁琐模式
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

#项目名称
project(clangPlugin-Var--runtime_c__TmPnt_ThreadLocal)


include("${CMAKE_CURRENT_SOURCE_DIR}/../ClAsCxx.cmake")

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal

#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的头文件路径
set(runtime_c__TmPnt_ThreadLocal__include "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include")
#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的静态库路径
set(runtime_c__TmPnt_ThreadLocal__staticLib "/fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a")

# libclangPlgVar_runtime_c_TmPnt_ThrLcl.a 开始
add_library(clangPlgVar_runtime_c_TmPnt_ThrLcl STATIC
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
)

target_include_directories(clangPlgVar_runtime_c_TmPnt_ThrLcl  PUBLIC
"${runtime_c__TmPnt_ThreadLocal__include}"
)
# libclangPlgVar_runtime_c_TmPnt_ThrLcl.a 结束

# _c__test_main.elf 开始
set(_c__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_C.c"
)
add_executable(_c__test_main.elf
${_c__SOURCES}
)
target_include_directories(_c__test_main.elf PRIVATE
"${runtime_c__TmPnt_ThreadLocal__include}"
)
# _c__test_main.elf 结束

# _cxx__test_main.elf 开始
set(_cxx__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_CXX.cpp"
)
add_executable(_cxx__test_main.elf
${_cxx__SOURCES}
)
target_include_directories( _cxx__test_main.elf PRIVATE
"${runtime_c__TmPnt_ThreadLocal__include}"
)
# _cxx__test_main.elf 结束

# _c__test_main_linkStaticArch.elf 开始
# 示范 c如何 使用 libclangPlgVar_runtime_c_TmPnt_ThrLcl.a
set(_c_LinkStaticArch__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_C.c"
)
add_executable( _c__test_main_linkStaticArch.elf
${_c_LinkStaticArch__SOURCES}
)
target_include_directories( _c__test_main_linkStaticArch.elf  PRIVATE
"${runtime_c__TmPnt_ThreadLocal__include}"
)
#指定链接静态库 libclangPlgVar_runtime_c_TmPnt_ThrLcl.a
target_link_libraries(_c__test_main_linkStaticArch.elf PRIVATE
"${runtime_c__TmPnt_ThreadLocal__staticLib}"
)
# _c__test_main_linkStaticArch.elf 结束



# _cxx__test_main_linkStaticArch.elf 开始
# 示范 c++如何 使用 libclangPlgVar_runtime_c_TmPnt_ThrLcl.a
set(_cxx_LinkStaticArch__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_CXX_LinkStaticArch.cpp"
)
add_executable(_cxx__test_main_linkStaticArch.elf
${_cxx_LinkStaticArch__SOURCES}
)
target_include_directories( _cxx__test_main_linkStaticArch.elf  PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)
#指定链接静态库 libclangPlgVar_runtime_c_TmPnt_ThrLcl.a
target_link_libraries(_cxx__test_main_linkStaticArch.elf  PRIVATE
"${runtime_c__TmPnt_ThreadLocal__staticLib}"
)
# _cxx__test_main_linkStaticArch.elf 结束
