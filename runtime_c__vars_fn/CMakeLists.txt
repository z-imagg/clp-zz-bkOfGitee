

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(runtime_c__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/runtime_c__vars_fn.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/test_main__runtime_c__vars_fn.c"
)

    add_executable(
    runtime_c__test_main
      ${runtime_c__SOURCES}
      )

    target_include_directories(
    runtime_c__test_main
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_c__vars_fn/include"
    )

