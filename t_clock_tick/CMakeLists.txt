#需要真实测试t_clock_tick.cpp :
# 1. thread_local 版本 t_clock_tick.cpp
# 2. c/c++混合
# 3. main.c 调用 thread_local版本 t_clock_tick.cpp

set(CTkTest_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../t_clock_tick/t_clock_tick.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../t_clock_tick/test_main.cpp"
)

add_executable(
CTkTest
    ${CTkTest_SOURCES}
)

target_include_directories(
CTkTest
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../t_clock_tick"
)