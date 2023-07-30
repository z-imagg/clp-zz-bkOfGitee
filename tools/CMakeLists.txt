

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clang-ctk/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clang-ctk/tools

set(CTkAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../tools/CTkMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/FuncDesc.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/FndCTkClROVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkAstAct.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkAstCnsm.cpp"
)

    # Create a library corresponding to 'plugin'
    add_executable(
        CTkAlone
      ${CTkAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    CTkAlone
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    CTkAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
