#include "Obfuscator/ObfuscatorMatcherForSub.h"

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
// ObfuscatorMatcherForSub - implementation
//-----------------------------------------------------------------------------
void ObfuscatorMatcherForSub::run(const MatchFinder::MatchResult &Result) {
  const auto &Op = Result.Nodes.getNodeAs<clang::BinaryOperator>("op");

  std::string LHSAsStr, RHSAsStr;
  SourceRange RangeLHS, RangeRHS;

  // Grab the left-hand-side of the expression
  if (const auto *LHSWithIC =
          Result.Nodes.getNodeAs<clang::ImplicitCastExpr>("lhs")) {
    const auto *LHS = LHSWithIC->IgnoreImplicit();

    LHSAsStr = dyn_cast<clang::DeclRefExpr>(LHS)->getDecl()->getName().str();
    RangeLHS = LHS->getSourceRange();
  } else if (const auto *LHS =
          Result.Nodes.getNodeAs<clang::IntegerLiteral>("lhs")) {
    LHSAsStr = std::to_string(LHS->getValue().getZExtValue());
    RangeLHS = LHS->getSourceRange();
  } else {
    llvm_unreachable("Unsupported case in the handler for `-`");
  }

  // Grab the right-hand-side of the expression
  if (const auto *RHSWithIC =
          Result.Nodes.getNodeAs<clang::ImplicitCastExpr>("rhs")) {
    const auto *RHS = RHSWithIC->IgnoreImplicit();

    RHSAsStr = dyn_cast<clang::DeclRefExpr>(RHS)->getDecl()->getName().str();
    RangeRHS = RHS->getSourceRange();
  } else if (const auto *RHS =
          Result.Nodes.getNodeAs<clang::IntegerLiteral>("rhs")) {
    RHSAsStr = std::to_string(RHS->getValue().getZExtValue());
    RangeRHS = RHS->getSourceRange();
  } else {
    llvm_unreachable("Unsupported case in the handler for `-`");
  }

  // Rewrite the expression
  ObfuscatorRewriter->ReplaceText(RangeLHS,
                                  "(" + LHSAsStr + " + ~" + RHSAsStr + ")");
  ObfuscatorRewriter->ReplaceText(Op->getOperatorLoc(), "+");
  ObfuscatorRewriter->ReplaceText(RangeRHS, "1");
}