//
// Created by zz on 2023/7/18.
//

#ifndef CLANG_TUTOR_LACOMMENTERMATCHER_H
#define CLANG_TUTOR_LACOMMENTERMATCHER_H


#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"

//-----------------------------------------------------------------------------
// ASTMatcher callback
//-----------------------------------------------------------------------------
class LACommenterMatcher
        : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    LACommenterMatcher(clang::Rewriter &LACRewriter) : LACRewriter(LACRewriter) {}
    // Callback that's executed whenever the Matcher in LACommenterASTConsumer
    // matches.
    void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;
    // Callback that's executed at the end of the translation unit
    void onEndOfTranslationUnit() override;

private:
    clang::Rewriter LACRewriter;
    llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

#endif //CLANG_TUTOR_LACOMMENTERMATCHER_H
