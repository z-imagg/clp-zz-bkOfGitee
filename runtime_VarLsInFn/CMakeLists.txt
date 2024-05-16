

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(runtime_VarLsInFn__SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_VarLsInFn/runtime_VarLsInFn.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../runtime_VarLsInFn/test_main__runtime.cpp"
)

    add_executable(
    runtime_VarLsInFn__test_main
      ${runtime_VarLsInFn__SOURCES}
      )

    target_include_directories(
    runtime_VarLsInFn__test_main
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime_VarLsInFn/include"
    )

