# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================
set(CLANG_TUTOR_PLUGINS
    LACommenter
    CodeStyleChecker
    Obfuscator
    UnusedForLoopVar
    CodeRefactor
    )

set(LACommenter_SOURCES
  LACommenter/LACommenterASTConsumer.cpp LACommenter/LACommenterMatcher.cpp LACommenter/LACPluginAction.cpp )

set(CodeStyleChecker_SOURCES
  CodeStyleChecker.cpp)

set(Obfuscator_SOURCES
  Obfuscator/ObfuscatorASTConsumer.cpp Obfuscator/ObfuscatorMatcherForAdd.cpp  Obfuscator/ObfuscatorMatcherForSub.cpp  Obfuscator/ObfuscatorPluginAction.cpp)

set(UnusedForLoopVar_SOURCES
  UnusedForLoopVar/UFLVMatcherPluginAction.cpp UnusedForLoopVar/UnusedForLoopVarASTConsumer.cpp UnusedForLoopVar/UnusedForLoopVarMatcher.cpp UnusedForLoopVar/UnusedForLoopVarVisitor.cpp)

set(CodeRefactor_SOURCES
  CodeRefactor/CodeRefactorAddPluginAction.cpp CodeRefactor/CodeRefactorASTConsumer.cpp CodeRefactor/CodeRefactorMatcher.cpp )

# CONFIGURE THE PLUGIN LIBRARIES
# ==============================
foreach( plugin ${CLANG_TUTOR_PLUGINS} )
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

    # On Darwin (unlike on Linux), undefined symbols in shared objects are not
    # allowed at the end of the link-edit. The plugins defined here:
    #  - _are_ shared objects
    #  - reference symbols from LLVM shared libraries, i.e. symbols which are
    #    undefined until those shared objects are loaded in memory (and hence
    #    _undefined_ during static linking)
    # The build will fail with errors like this:
    #    "Undefined symbols for architecture x86_64"
    # with various LLVM symbols being undefined. Since those symbols are later
    # loaded and resolved at runtime, these errors are false positives.
    # This behaviour can be modified via the '-undefined' OS X linker flag as
    # follows.
    target_link_libraries(
      ${plugin}
      "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
      )
endforeach()
