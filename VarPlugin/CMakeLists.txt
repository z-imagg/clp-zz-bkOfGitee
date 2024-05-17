# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================


set(Var_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarAstAct.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/StrUtil.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/FnVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/RetVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarDeclVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/CollectIncMacro_PPCb.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/VarAstCnsm.cpp"
)


    # Create a library corresponding to 'plugin'
    add_library(
            VarPlugin
      SHARED
      ${Var_SOURCES}
      )

target_link_libraries(VarPlugin
        "${LIBFMT_STATIC}"   #/pubx/fmt/build/libfmt.a
        )

    # Configure include directories for 'plugin'
    target_include_directories(VarPlugin
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
      "${LIBFMT_INCLUDE}"
    )

