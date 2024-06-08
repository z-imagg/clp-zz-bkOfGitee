

MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
#CMAKE_SOURCE_DIR: /pubx/clp-zz/

MESSAGE("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#CMAKE_CURRENT_SOURCE_DIR: /pubx/clp-zz/tools

#导入变量 base_util__SOURCES
include("${CMAKE_CURRENT_SOURCE_DIR}/../base_home/base_util.cmake")

set(TestAlone_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/../TestAlone/TestAloneMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/RangeHasMacroAstVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/Var/CollectIncMacro_PPCb.cpp"
"${base_util__SOURCES}"

)

    # Create a library corresponding to 'plugin'
    add_executable(
        TestAlone
      ${TestAlone_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
    TestAlone
      PRIVATE
    "${LIBFMT_INCLUDE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"  # == /fridaAnlzAp/clp-zz/include
    "${CMAKE_CURRENT_SOURCE_DIR}/../base_home/include/"
    )

    # Link in the required libraries
    target_link_libraries(
    TestAlone
      "clangTooling"
   "${LIBFMT_STATIC}"  #/pubx/fmt/build/libfmt.a
    )
