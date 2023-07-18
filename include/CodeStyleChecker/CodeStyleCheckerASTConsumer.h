#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"


#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer {
public:
    explicit CodeStyleCheckerASTConsumer(clang::ASTContext *Context,
                                         bool MainFileOnly,
                                         clang::SourceManager &SM)
            : Visitor(Context), SM(SM), MainTUOnly(MainFileOnly) {}

    void HandleTranslationUnit(clang::ASTContext &Ctx) {
      if (!MainTUOnly)
        Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
      else {
        // Only visit declarations declared in the input TU
        auto Decls = Ctx.getTranslationUnitDecl()->decls();
        for (auto &Decl : Decls) {
          // Ignore declarations out of the main translation unit.
          //
          // SourceManager::isInMainFile method takes into account locations
          // expansion like macro expansion scenario and checks expansion
          // location instead if spelling location if required.
          if (!SM.isInMainFile(Decl->getLocation()))
            continue;
          Visitor.TraverseDecl(Decl);
        }
      }
    }

private:
    CodeStyleCheckerVisitor Visitor;
    clang::SourceManager &SM;
    // Should this plugin be only run on the main translation unit?
    bool MainTUOnly = true;
};
