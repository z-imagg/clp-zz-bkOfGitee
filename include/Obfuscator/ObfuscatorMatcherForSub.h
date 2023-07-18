
#ifndef CLANG_TUTOR_OBFUSCATORMATCHERFORSUB_H
#define CLANG_TUTOR_OBFUSCATORMATCHERFORSUB_H


#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <memory>



//-----------------------------------------------------------------------------
// ASTMatcher callback (sub instructions)
//-----------------------------------------------------------------------------
class ObfuscatorMatcherForSub
        : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    explicit ObfuscatorMatcherForSub(
            std::shared_ptr<clang::Rewriter> RewriterForObfuscatorSub)
            : ObfuscatorRewriter{std::move(RewriterForObfuscatorSub)} {}

    void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
    std::shared_ptr<clang::Rewriter> ObfuscatorRewriter;
};

#endif //CLANG_TUTOR_OBFUSCATORMATCHERFORSUB_H
