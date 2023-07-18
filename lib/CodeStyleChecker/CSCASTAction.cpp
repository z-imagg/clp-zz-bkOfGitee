//==============================================================================
// FILE:
//    CodeStyleChecker.cpp
//
// DESCRIPTION:
//    Checks whether function, variable and type names follow the LLVM's coding
//    style guide. If not, issue a warning and generate a FixIt hint. The
//    following items are exempt from the above rules and are ignored:
//      * anonymous fields in classes and structs
//      * anonymous unions
//      * anonymous function parameters
//      * conversion operators
//    These exemptions are further documented in the source code below.
//
//    This plugin is complete in the sense that it successfully processes
//    vector.h from STL. Also, note that it implements only a small subset of
//    LLVM's coding guidelines.
//
//    By default this plugin will only run on the main translation unit. Use
//    `-main-tu-only=false` to make it run on e.g. included header files too.
//
// USAGE:
//    1. As a loadable Clang plugin:
//    Main TU only:
//      * clang -cc1 -load <BUILD_DIR>/lib/libCodeStyleChecker.dylib -plugin '\'
//        CSC test/CodeStyleCheckerVector.cpp
//    All TUs (the main file and the #includ-ed header files)
//      * clang -cc1 -load <BUILD_DIR>/lib/libCodeStyleChecker.dylib '\'
//        -plugin CSC -plugin-arg-CSC -main-tu-only=false '\'
//        test/CodeStyleCheckerVector.cpp
//    2. As a standalone tool:
//        <BUILD_DIR>/bin/ct-code-style-checker '\'
//        test/ct-code-style-checker-basic.cpp
//
// License: The Unlicense
//==============================================================================
#include "CodeStyleChecker.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;


//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class CSCASTAction : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &Compiler,
                      llvm::StringRef InFile) override {
      return std::make_unique<CodeStyleCheckerASTConsumer>(
              &Compiler.getASTContext(), MainTuOnly, Compiler.getSourceManager());
    }

    bool ParseArgs(const CompilerInstance &CI,
                   const std::vector<std::string> &Args) override {
      for (StringRef Arg : Args) {
        if (Arg.startswith("-main-tu-only="))
          MainTuOnly =
                  Arg.substr(strlen("-main-tu-only=")).equals_insensitive("true");
        else if (Arg.startswith("-help"))
          PrintHelp(llvm::errs());
        else
          return false;
      }

      return true;
    }

    void PrintHelp(llvm::raw_ostream &ros) {
      ros << "Help for CodeStyleChecker plugin goes here\n";
    }

private:
    bool MainTuOnly = true;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static clang::FrontendPluginRegistry::Add<CSCASTAction>
        X(/*Name=*/"CSC",
        /*Description=*/"Checks whether class, variable and function names "
                        "adhere to LLVM's guidelines");