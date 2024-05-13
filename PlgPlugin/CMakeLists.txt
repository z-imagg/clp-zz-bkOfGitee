# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================


set(Plg_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Plg/PlgAstAct.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Plg/PlgVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Plg/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Plg/CollectIncMacro_PPCb.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Plg/PlgAstCnsm.cpp"
)


    # Create a library corresponding to 'plugin'
    add_library(
            PlgPlugin
      SHARED
      ${Plg_SOURCES}
      )

target_link_libraries(PlgPlugin
        "${LIBFMT_STATIC}"   #/pubx/fmt/build/libfmt.a
        )

    # Configure include directories for 'plugin'
    target_include_directories(PlgPlugin
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
      "${LIBFMT_INCLUDE}"
    )

