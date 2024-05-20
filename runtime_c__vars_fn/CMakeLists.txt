
cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_C_STANDARD 11)

#Makefile繁琐模式
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

#项目名称
project(clangPlugin-Var--runtime_c__vars_fn)


include("${CMAKE_CURRENT_SOURCE_DIR}/../ClAsCxx.cmake")

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(runtime_c__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/runtime_c__vars_fn.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/test_main__runtime_c__vars_fn.c"
"/app/antirez--sds/sds.c"
)

add_executable(
runtime_c__test_main
  ${runtime_c__SOURCES}
)

target_include_directories(
runtime_c__test_main
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/include"
    "/app/clibs--list/src"
    "/app/antirez--sds/"
)
target_link_libraries(
runtime_c__test_main
    "/app/clibs--list/build/libclibs_list.a"
)


# 添加静态库目标
add_library(clangPlgVar_runtime_c STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/runtime_c__vars_fn.c"
    "/app/antirez--sds/sds.c"
    )

# 设置静态库目标的包含目录
target_include_directories(clangPlgVar_runtime_c
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/include"
    "/app/clibs--list/src"
    "/app/antirez--sds/"
    )



