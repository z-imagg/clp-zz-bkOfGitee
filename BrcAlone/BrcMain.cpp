#include <clang/Frontend/FrontendActions.h>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "Brc/BrcAstCnsm.h"
#include "base/ActMain.h"

using namespace llvm;
using namespace clang;

static llvm::cl::OptionCategory BrcAloneCategory("BrcAlone选项");

class _BrcAstAct : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &CI,
                      llvm::StringRef inFile) override {
      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      ASTContext &astContext = CI.getASTContext();
      //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
      CI.getDiagnostics().setSourceManager(&SM);
      mRewriter_ptr->setSourceMgr(SM, langOptions);//A

      //Act中 是 每次都是 新创建 AddBraceAstCnsm
      return std::make_unique<BrcAstCnsm>(CI,mRewriter_ptr, &astContext, SM, langOptions);
    }
private:
    const std::shared_ptr<Rewriter> mRewriter_ptr=std::make_shared<Rewriter>();//这里是插件Act中的Rewriter，是源头，理应构造Rewriter.
};



int main(int Argc, const char **Argv) {
  const std::unique_ptr<tooling::FrontendActionFactory> &frontendActionFactory = clang::tooling::newFrontendActionFactory<_BrcAstAct>();
  int Result =   act_main(Argc,Argv,BrcAloneCategory,frontendActionFactory,"加花括号插件", false);
  return Result;
}

