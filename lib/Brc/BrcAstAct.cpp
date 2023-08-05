#include "Brc/BrcAstCnsm.h"

#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace llvm;
using namespace clang;

class BrcAstAct : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &CI,
                      llvm::StringRef inFile) override {
      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      ASTContext &astContext = CI.getASTContext();
      CI.getDiagnostics().setSourceManager(&SM);
      mRewriter_ptr->setSourceMgr(SM, langOptions);


      return std::make_unique<BrcAstCnsm>(CI,mRewriter_ptr, &astContext, SM, langOptions);
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
    const std::shared_ptr<Rewriter> mRewriter_ptr=std::make_shared<Rewriter>();//这里是插件Act中的Rewriter，是源头，理应构造Rewriter.
};

static FrontendPluginRegistry::Add<BrcAstAct>   actRegistry(/*Name=*/"BrcPlugin",  /*Description=*/"加花括号插件");