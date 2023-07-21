# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================
set(CTk_PLUGINS
        CTk
    )


set(CTk_SOURCES
        CTk/CTkAstAct.cpp CTk/CTkVst.cpp CTk/FndCTkClROVst.cpp)




# CONFIGURE THE PLUGIN LIBRARIES
# ==============================
foreach( plugin ${CTk_PLUGINS} )
    # Create a library corresponding to 'plugin'
    add_library(
      ${plugin}
      SHARED
      ${${plugin}_SOURCES}
      )

    # Configure include directories for 'plugin'
    target_include_directories(
      ${plugin}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

endforeach()
