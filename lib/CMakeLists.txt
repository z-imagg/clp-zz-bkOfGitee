# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================


set(CTk_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/FndCTkClROVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkAstAct.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkAstCnsm.cpp"
)


    # Create a library corresponding to 'plugin'
    add_library(
    CTk
      SHARED
      ${CTk_SOURCES}
      )

target_link_libraries(CTk
        "${LIBFMT_STATIC}"   #/pubx/fmt/build/libfmt.a
        )

    # Configure include directories for 'plugin'
    target_include_directories(
    CTk
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
      "${LIBFMT_INCLUDE}"
    )

