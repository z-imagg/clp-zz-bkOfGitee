//
// Created by zz on 2023/7/18.
//

#ifndef CLANG_TUTOR_CODEREFACTORASTCONSUMER_H
#define CLANG_TUTOR_CODEREFACTORASTCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include "CodeRefactor/CodeRefactorMatcher.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeRefactorASTConsumer : public clang::ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    CodeRefactorASTConsumer(clang::Rewriter &R, std::string ClassName,
                            std::string OldName, std::string NewName);
    void HandleTranslationUnit(clang::ASTContext &Ctx) override {
      Finder.matchAST(Ctx);
    }

private:
    clang::ast_matchers::MatchFinder Finder;
    CodeRefactorMatcher CodeRefactorHandler;
    // The name of the class to match. Use base class name to rename in all
    // derived classes.
    std::string ClassName;
    // The name of the member method to match
    std::string OldName;
    // The new name of the member method
    std::string NewName;
};


#endif //CLANG_TUTOR_CODEREFACTORASTCONSUMER_H
