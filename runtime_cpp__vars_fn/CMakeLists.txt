

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(runtime_cpp__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/runtime_cpp__vars_fn.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/test_main__runtime_cpp__vars_fn.cpp"
)

    add_executable(
    runtime_cpp__test_main
      ${runtime_cpp__SOURCES}
      )

    target_include_directories(
    runtime_cpp__test_main
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/include"
    )

# 添加静态库目标
add_library(clangPlgVar_runtime_cxx STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/runtime_cpp__vars_fn.cpp"
        )

# 设置静态库目标的包含目录
target_include_directories(clangPlgVar_runtime_cxx
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_cpp__vars_fn/include"
        )