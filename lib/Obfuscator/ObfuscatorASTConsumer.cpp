
#include "Obfuscator/ObfuscatorASTConsumer.h"

#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>

using namespace clang;
using namespace ast_matchers;





//-----------------------------------------------------------------------------
// ObfuscatorASTConsumer - implementation
//-----------------------------------------------------------------------------
ObfuscatorASTConsumer::ObfuscatorASTConsumer(std::shared_ptr<Rewriter> R)
        : AddHandler(R), SubHandler(R) {
  const auto MatcherAdd =
          binaryOperator(
                  hasOperatorName("+"),
                  hasLHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("lhs"),
                               integerLiteral().bind("lhs"))),
                  hasRHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("rhs"),
                               integerLiteral().bind("rhs"))))
                  .bind("op");

  const auto MatcherSub =
          binaryOperator(
                  hasOperatorName("-"),
                  hasLHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("lhs"),
                               integerLiteral().bind("lhs"))),
                  hasRHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("rhs"),
                               integerLiteral().bind("rhs"))))
                  .bind("op");

  Matcher.addMatcher(MatcherAdd, &AddHandler);
  Matcher.addMatcher(MatcherSub, &SubHandler);
}