//==============================================================================
// FILE:
//    Obfuscator.h
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_OBFUSCATORASTCONSUMER_H
#define CLANG_TUTOR_OBFUSCATORASTCONSUMER_H


#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <memory>

#include "Obfuscator/ObfuscatorMatcherForAdd.h"
#include "Obfuscator/ObfuscatorMatcherForSub.h"


//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class ObfuscatorASTConsumer : public clang::ASTConsumer {
public:
    ObfuscatorASTConsumer(std::shared_ptr<clang::Rewriter> R);
    void HandleTranslationUnit(clang::ASTContext &Ctx) override {
      Matcher.matchAST(Ctx);
    }

private:
    clang::ast_matchers::MatchFinder Matcher;
    ObfuscatorMatcherForAdd AddHandler;
    ObfuscatorMatcherForSub SubHandler;
};

#endif //CLANG_TUTOR_OBFUSCATORASTCONSUMER_H
