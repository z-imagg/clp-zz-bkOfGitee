
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
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/TestMain_C.c"
"/app/antirez--sds/sds.c"
)

#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal 的头文件路径
set(runtime_c__TmPnt_ThreadLocal__include "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__TmPnt_ThreadLocal/include")

#测试main 开始
add_executable(
runtime_c__test_main
  ${runtime_c__SOURCES}
)

target_include_directories(
runtime_c__test_main
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/include"
    "${runtime_c__TmPnt_ThreadLocal__include}"
    "/app/clibs--list/src"
    "/app/antirez--sds/"
)
target_link_libraries(
runtime_c__test_main
    "/app/clibs--list/build/libclibs_list.a"
    "/fridaAnlzAp/clang-var/runtime_c__TmPnt_ThreadLocal/build/libclangPlgVar_runtime_c_TmPnt_ThrLcl.a"
)
#测试main 结束

#静态库 开始
# 添加静态库目标
add_library(clangPlgVar_runtime_c STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/runtime_c__vars_fn.c"
    "/app/antirez--sds/sds.c"
    )

# 设置静态库目标的包含目录
target_include_directories(clangPlgVar_runtime_c
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/include"
#本c运行时 依赖模块 runtime_c__TmPnt_ThreadLocal， 所以依赖 本c运行时 的应用 一定要添加依赖 runtime_c__TmPnt_ThreadLocal
    "${runtime_c__TmPnt_ThreadLocal__include}"
    "/app/clibs--list/src"
    "/app/antirez--sds/"
    )
#静态库 结束


