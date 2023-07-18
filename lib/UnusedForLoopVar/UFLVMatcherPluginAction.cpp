//==============================================================================
// FILE:
//  UnusedForLoopVar.cpp
//
// DESCRIPTION:
//
// USAGE:
//   * clang -cc1 -load <BUILD_DIR>/lib/libUnusedForLoopVar.dylib '\'
//      -plugin UFLV test/UnusedForLoopVar_nested.cpp
//
// License: The Unlicense
//==============================================================================
#include "UnusedForLoopVar/UnusedForLoopVarASTConsumer.h"

#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;




//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class UFLVMatcherPluginAction : public PluginASTAction {
public:
    // Our plugin can alter behavior based on the command line options
    bool ParseArgs(const CompilerInstance &,
                   const std::vector<std::string> &) override {
      return true;
    }

    // Returns our ASTConsumer per translation unit.
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef file) override {
      return std::make_unique<UnusedForLoopVarASTConsumer>(CI.getASTContext(),
                                                           CI.getSourceManager());
    }
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<UFLVMatcherPluginAction>
        X(/*Name=*/"UFLV",
        /*Desc=*/"Find unused for-loop variables");