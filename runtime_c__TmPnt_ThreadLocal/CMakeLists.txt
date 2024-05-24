
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

#c测试main
set(_c__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_C.c"
)
add_executable(
_c__test_main
  ${_c__SOURCES}
)
target_include_directories(
_c__test_main
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)

#c++测试main
set(_cxx__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/TestMain_CXX.cpp"
)
add_executable(
_cxx__test_main
  ${_cxx__SOURCES}
)
target_include_directories(
_cxx__test_main
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
)


# 添加静态库目标
add_library(clangPlgVar_runtime_cTmPntThrLcl STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/rntm_c__TmPnt_ThrLcl.c"
    )

# 设置静态库目标的包含目录
target_include_directories(clangPlgVar_runtime_cTmPntThrLcl
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include"
    )



