

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clang-ctk/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clang-ctk/tools

set(BrcAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../BrcAlone/BrcMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/BrcVst.cpp"
#"${CMAKE_CURRENT_SOURCE_DIR}/../include/Brc/BrcAstCnsm.h"
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
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    BrcAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
