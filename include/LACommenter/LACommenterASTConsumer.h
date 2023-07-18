//
// Created by zz on 2023/7/18.
//

#ifndef CLANG_TUTOR_LACOMMENTERASTCONSUMER_H
#define CLANG_TUTOR_LACOMMENTERASTCONSUMER_H


#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "LACommenter/LACommenterMatcher.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class LACommenterASTConsumer : public clang::ASTConsumer {
public:
    LACommenterASTConsumer(clang::Rewriter &R);
    void HandleTranslationUnit(clang::ASTContext &Ctx) override {
      Finder.matchAST(Ctx);
    }

private:
    clang::ast_matchers::MatchFinder Finder;
    LACommenterMatcher LACHandler;
};

#endif //CLANG_TUTOR_LACOMMENTERASTCONSUMER_H
