# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================


set(CTk_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/BrcVst.cpp"
#"${CMAKE_CURRENT_SOURCE_DIR}/../include/Brc/BrcAstCnsm.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Brc/BrcAstCnsm.cpp"
)


    # Create a library corresponding to 'plugin'
    add_library(
            Brc
      SHARED
      ${CTk_SOURCES}
      )

target_link_libraries(Brc
        "${LIBFMT_STATIC}"   #/pubx/fmt/build/libfmt.a
        )

    # Configure include directories for 'plugin'
    target_include_directories(
            Brc
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
      "${LIBFMT_INCLUDE}"
    )

