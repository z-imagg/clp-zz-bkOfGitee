//
// Created by zz on 2023/7/18.
//

#ifndef CLANG_TUTOR_CODEREFACTORMATCHER_H
#define CLANG_TUTOR_CODEREFACTORMATCHER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"

//-----------------------------------------------------------------------------
// ASTFinder callback
//-----------------------------------------------------------------------------
class CodeRefactorMatcher
        : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    explicit CodeRefactorMatcher(clang::Rewriter &RewriterForCodeRefactor,
                                 std::string NewName)
            : CodeRefactorRewriter(RewriterForCodeRefactor), NewName(NewName) {}
    void onEndOfTranslationUnit() override;

    void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
    clang::Rewriter CodeRefactorRewriter;
    // The new name of the matched member expression.
    // NOTE: This matcher already knows *what* to replace (which method in which
    // class/struct), because it _matched_ a member expression that corresponds to
    // the command line arguments.
    std::string NewName;
};


#endif //CLANG_TUTOR_CODEREFACTORMATCHER_H
