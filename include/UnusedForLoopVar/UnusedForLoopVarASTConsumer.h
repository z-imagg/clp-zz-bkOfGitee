
#ifndef CLANG_TUTOR_UNUSEDFORLOOPVARASTCONSUMER_H
#define CLANG_TUTOR_UNUSEDFORLOOPVARASTCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"


#include "UnusedForLoopVar/UnusedForLoopVarMatcher.h"
#include "UnusedForLoopVar/UnusedForLoopVarVisitor.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class UnusedForLoopVarASTConsumer : public clang::ASTConsumer {
public:
    UnusedForLoopVarASTConsumer(clang::ASTContext &Ctx, clang::SourceManager &SM);
    void HandleTranslationUnit(clang::ASTContext &Ctx) override {
      Matcher.matchAST(Ctx);

      // Only visit declarations declared in the input TU
      auto Decls = Ctx.getTranslationUnitDecl()->decls();
      for (auto &Decl : Decls) {
        if (!SM.isInMainFile(Decl->getLocation()))
          continue;
        UFLVVisitor.TraverseDecl(Decl);
      }
    }

private:
    clang::ast_matchers::MatchFinder Matcher;
    clang::SourceManager &SM;

    UnusedForLoopVarMatcher UFLVMatcher;
    UnusedForLoopVarVisitor UFLVVisitor;
};


#endif //CLANG_TUTOR_UNUSEDFORLOOPVARASTCONSUMER_H
