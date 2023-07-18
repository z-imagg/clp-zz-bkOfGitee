//==============================================================================
// FILE:
//    LACommenter.cpp
//
// DESCRIPTION:
//    Literal argument commenter - adds in-line C-style comments as recommended
//    in LLVM's coding guideline:
//      * https://llvm.org/docs/CodingStandards.html#comment-formatting
//    This plugin will comment the following literal argument types:
//      * integer, character, floating, boolean, string
//
//    Below is an example for a function that takes one integer argument:
//    ```c
//    extern void foo(int some_arg);
//
//    void(bar) {
//      foo(/*some_arg=*/123);
//    }
//    ```
//
// TODO: Update the following
//    This plugin modifies the input file in-place.
//
// USAGE:
//    1. As a loadable Clang plugin:
//      clang -cc1 -load <BUILD_DIR>/lib/libLACommenter.dylib '\'
//        -plugin LACPlugin test/LACInt.cpp
//    2. As a standalone tool:
//      <BUILD_DIR>/bin/ct-la-commenter test/LACInt.cpp
//
// REFERENCES:
//    Based on an example by Peter Smith:
//      * https://s3.amazonaws.com/connect.linaro.org/yvr18/presentations/yvr18-223.pdf
//
// License: The Unlicense
//==============================================================================
#include "LACommenter/LACommenterASTConsumer.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace ast_matchers;



//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class LACPluginAction : public PluginASTAction {
public:
    // Our plugin can alter behavior based on the command line options
    bool ParseArgs(const CompilerInstance &,
                   const std::vector<std::string> &) override {
      return true;
    }

    // Returns our ASTConsumer per translation unit.
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef file) override {
      //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
      RewriterForLAC.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
      //Rewriter:3:  Action将Rewriter传递给Consumer
      return std::make_unique<LACommenterASTConsumer>(RewriterForLAC);
    }

private:
    //Rewriter:0:  Rewriter总是作为Action类中的一个成员字段.
    //Rewriter:1:  Rewriter并不是上层传递下来的，而是自己在这构造的.
    Rewriter RewriterForLAC;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<LACPluginAction>
        X(/*Name=*/"LAC",
        /*Desc=*/"Literal Argument Commenter");