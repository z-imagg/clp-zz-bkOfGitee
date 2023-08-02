#include <clang/Frontend/FrontendActions.h>
#include "Brc/BrcAstCnsm.h"
#include "Brc/FndBrcFlagCmtHdl.h"

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
//class MyCommentAct : public SyntaxOnlyAction {
class MyCommentAct : public SyntaxOnlyAction {
public:
protected:
    void ExecuteAction() override {
      CompilerInstance &CI = getCompilerInstance();
//      const LangOptions &langOptions=CI.getLangOpts();
//      SourceManager &SM=CI.getSourceManager();
      CI.getPreprocessor().addCommentHandler(new FndBrcFlagCmtHdl(CI));
//      ASTFrontendAction::ExecuteAction();
      SyntaxOnlyAction::ExecuteAction();

    }

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

      /**
时间轴正向: C--->C'--->C'', 即'越多时刻越晚.
各个C、C'、C''、C'''处的代码都是 Rewriter.overwriteChangedFiles()
       */
//      mRewriter_ptr->overwriteChangedFiles();//C处 正常.

//      const std::shared_ptr<Rewriter> &rewriter_ptr = std::make_shared<Rewriter>();
      //Rewriter:3:  Action将Rewriter传递给Consumer
      //Act中 是 每次都是 新创建 AddBraceAstCnsm
      return std::make_unique<AddBraceAstCnsm>(CI, mRewriter_ptr,
                                               &astContext, SM, langOptions);
    }




//    void EndSourceFileAction() override { }  //   貌似有时候并没有调用EndSourceFileAction，因此去掉

private:
    const std::shared_ptr<Rewriter> mRewriter_ptr=std::make_shared<Rewriter>();//这里是插件Act中的Rewriter，是源头，理应构造Rewriter.
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<BrcAstAct>
        X(/*Name=*/"Brc",
        /*Description=*/"加花括号插件");