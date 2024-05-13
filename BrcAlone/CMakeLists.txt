

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clang-brc/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clang-brc/tools

set(VarAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/ActMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../VarAlone/VarMain.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/CollectIncMacro_PPCb.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarAstCnsm.cpp"
)

    # Create a library corresponding to 'plugin'
    add_executable(
        VarAlone
      ${VarAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    VarAlone
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    VarAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
