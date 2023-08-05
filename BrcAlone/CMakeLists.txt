

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clang-brc/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clang-brc/tools

set(BrcAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/ActMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../BrcAlone/BrcMain.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/BrcVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/BrcAstCnsm.cpp"
)

    # Create a library corresponding to 'plugin'
    add_executable(
        BrcAlone
      ${BrcAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    BrcAlone
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    BrcAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
