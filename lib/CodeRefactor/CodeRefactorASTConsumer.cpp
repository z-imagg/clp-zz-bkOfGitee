#include "CodeRefactor/CodeRefactorASTConsumer.h"

#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/Refactoring/Rename/RenamingAction.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace clang;
using namespace ast_matchers;


CodeRefactorASTConsumer::CodeRefactorASTConsumer(Rewriter &R,
                                                 std::string ClassName,
                                                 std::string OldName,
                                                 std::string NewName)
        //Rewriter:4:  Consumer将Rewriter传递给Matcher
        : CodeRefactorHandler(R, NewName),

        ClassName(ClassName), OldName(OldName),
          NewName(NewName) {

  const auto MatcherForMemberAccess = cxxMemberCallExpr(
          callee(memberExpr(member(hasName(OldName))).bind("MemberAccess")),
          thisPointerType(cxxRecordDecl(isSameOrDerivedFrom(hasName(ClassName)))));

  Finder.addMatcher(MatcherForMemberAccess, &CodeRefactorHandler);

  const auto MatcherForMemberDecl = cxxRecordDecl(
          allOf(isSameOrDerivedFrom(hasName(ClassName)),
                hasMethod(decl(namedDecl(hasName(OldName))).bind("MemberDecl"))));

  Finder.addMatcher(MatcherForMemberDecl, &CodeRefactorHandler);
}
