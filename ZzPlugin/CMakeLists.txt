# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================


set(Var_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/VarAstAct.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/ASTContextUtil.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/StrUtil.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/FnVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/RetVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/VarDeclVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/CollectIncMacro_PPCb.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/VarAstCnsm.cpp"
)


    # Create a library corresponding to 'plugin'
    add_library(
            ZzPlugin
      SHARED
      ${Var_SOURCES}
      )

target_link_libraries(ZzPlugin
        "${LIBFMT_STATIC}"   #/pubx/fmt/build/libfmt.a
        )

    # Configure include directories for 'plugin'
    target_include_directories(ZzPlugin
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
      "${LIBFMT_INCLUDE}"
    )

