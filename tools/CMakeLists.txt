set(CLANG_TUTOR_TOOLS
CTkAlone
)



set(CTkAlone_SOURCES
 "${CMAKE_CURRENT_SOURCE_DIR}/CTkMain.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/Util.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/FndCTkClROVst.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../lib/CTk/CTkAstAct.cpp"
)

# CONFIGURE THE TOOLS
# ===================
foreach( tool ${CLANG_TUTOR_TOOLS} )
    # Create a library corresponding to 'plugin'
    add_executable(
      ${tool}
      ${${tool}_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
      ${tool}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    # Link in the required libraries 
    target_link_libraries(
      ${tool}
      "clangTooling"
    )
endforeach()
