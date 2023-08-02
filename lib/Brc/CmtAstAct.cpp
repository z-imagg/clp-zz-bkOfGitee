#include "Brc/BrcAstCnsm.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "Brc/CmtAstCnsm.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class CmtAstAct : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance &CI,
                      llvm::StringRef inFile) override {
      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      ASTContext &astContext = CI.getASTContext();
      //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
      CI.getDiagnostics().setSourceManager(&SM);
//      mRewriter_ptr->setSourceMgr(SM, langOptions);//A

      //Act中 是 每次都是 新创建 AddBraceAstCnsm
      return std::make_unique<CmtAstCnsm>(CI,  &astContext, SM, langOptions);
    }




};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<CmtAstAct>
        cmtAstActRegistry( "Brc", "加花括号插件");