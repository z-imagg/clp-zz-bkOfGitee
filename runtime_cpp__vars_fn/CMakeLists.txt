

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

