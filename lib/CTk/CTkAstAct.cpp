#include "CTk/CTkAstCnsm.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class CTkAstAct : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &CI,
                      llvm::StringRef inFile) override {
      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      ASTContext &astContext = CI.getASTContext();
      //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
      mRewriter.setSourceMgr(SM, langOptions);

      //Rewriter:3:  Action将Rewriter传递给Consumer
      return std::make_unique<CTkAstCnsm>(CI,mRewriter,
                                                           &astContext, SM, langOptions);
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
static FrontendPluginRegistry::Add<CTkAstAct>
        X(/*Name=*/"CTk",
        /*Description=*/"Checks whether class, variable and function names "
                        "adhere to LLVM's guidelines");