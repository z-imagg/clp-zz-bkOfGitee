
MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(TestIn_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../test_in/test_main.cpp"
    MyClz.cpp
)

add_executable(
    TestIn
    ${TestIn_SOURCES}
)


