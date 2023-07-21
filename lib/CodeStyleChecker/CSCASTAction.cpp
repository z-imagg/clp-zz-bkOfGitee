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
#include "CodeStyleChecker/CodeStyleCheckerASTConsumer.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;


//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class CSCASTAction : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &Compiler,
                      llvm::StringRef InFile) override {
      //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
      mRewriter.setSourceMgr(Compiler.getSourceManager(), Compiler.getLangOpts());

      //Rewriter:3:  Action将Rewriter传递给Consumer
      return std::make_unique<CodeStyleCheckerASTConsumer>(mRewriter,
              &Compiler.getASTContext(), Compiler.getSourceManager(),Compiler.getLangOpts());
    }

    bool ParseArgs(const CompilerInstance &CI,
                   const std::vector<std::string> &Args) override {


      return true;
    }


    PluginASTAction::ActionType getActionType() override {
      //本插件自动运行:  在MainAction后运行本插件
      return AddAfterMainAction;
    }

//    void EndSourceFileAction() override { }  //   貌似有时候并没有调用EndSourceFileAction，因此去掉

private:
    //Rewriter:0:  Rewriter总是作为Action类中的一个成员字段.
    //Rewriter:1:  Rewriter并不是上层传递下来的，而是自己在这构造的.
    Rewriter mRewriter;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static clang::FrontendPluginRegistry::Add<CSCASTAction>
        X(/*Name=*/"CSC",
        /*Description=*/"Checks whether class, variable and function names "
                        "adhere to LLVM's guidelines");