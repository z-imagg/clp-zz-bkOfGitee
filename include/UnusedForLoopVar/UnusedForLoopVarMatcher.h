
#ifndef CLANG_TUTOR_UNUSEDFORLOOPVARMATCHER_H
#define CLANG_TUTOR_UNUSEDFORLOOPVARMATCHER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"

//-----------------------------------------------------------------------------
// ASTMatcher callback
//-----------------------------------------------------------------------------
class UnusedForLoopVarMatcher
        : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    UnusedForLoopVarMatcher() = default;
    // Callback that's executed whenever the Matcher in
    // UnusedForLoopVarASTConsumer matches
    void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
    void runRegularForLoop(const clang::ast_matchers::MatchFinder::MatchResult &,
                           clang::ASTContext *Ctx);
    void runRangeForLoop(const clang::ast_matchers::MatchFinder::MatchResult &,
                         clang::ASTContext *Ctx);
};


#endif //CLANG_TUTOR_UNUSEDFORLOOPVARMATCHER_H
