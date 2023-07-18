#include <clang/Rewrite/Core/Rewriter.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"


#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit CodeStyleCheckerASTConsumer(clang::Rewriter &R, clang::ASTContext *Context, bool MainFileOnly,
                                         clang::SourceManager &SM)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            : Visitor(R, Context),
            SM(SM), MainTUOnly(MainFileOnly) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Ctx) override{
        Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
/*        auto Decls = Ctx.getTranslationUnitDecl()->decls();
        for (auto &Decl : Decls) { //貌似不需要此循环
          if (!SM.isInMainFile(Decl->getLocation()))
            continue;
          Visitor.TraverseDecl(Decl);
        }*/

      Visitor.mRewriter.getEditBuffer(SM.getMainFileID())
              .write(llvm::outs());
    }

private:
    CodeStyleCheckerVisitor Visitor;
    clang::SourceManager &SM;
    // Should this plugin be only run on the main translation unit?
    bool MainTUOnly = true;
};
