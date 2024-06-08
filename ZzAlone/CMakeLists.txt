

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clp-zz/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clp-zz/tools

set(ZzAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/ActMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../ZzAlone/VarMain.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/StrUtil.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/ASTContextUtil.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../base_home/impl/base/LocId.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/FnVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/RetVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/VarDeclVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/CollectIncMacro_PPCb.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Zz/ZzAstCnsm.cpp"
)

    # Create a library corresponding to 'plugin'
    add_executable(
        ZzAlone
      ${ZzAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    ZzAlone
      PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include"
    "${LIBFMT_INCLUDE}"
    )

    # Link in the required libraries
    target_link_libraries(
    ZzAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
