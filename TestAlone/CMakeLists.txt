

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clang-ctk/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clang-ctk/tools

set(TestAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../TestAlone/TestAloneMain.cpp"

)

    # Create a library corresponding to 'plugin'
    add_executable(
        TestAlone
      ${TestAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    TestAlone
      PRIVATE
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    TestAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
