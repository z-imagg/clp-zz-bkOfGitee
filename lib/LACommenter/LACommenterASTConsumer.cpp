
#include "LACommenter.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace ast_matchers;


LACommenterASTConsumer::LACommenterASTConsumer(Rewriter &R) : LACHandler(R) {
  StatementMatcher CallSiteMatcher =
          callExpr(
                  allOf(callee(functionDecl(unless(isVariadic())).bind("callee")),
                        unless(cxxMemberCallExpr(
                                on(hasType(substTemplateTypeParmType())))),
                        anyOf(hasAnyArgument(ignoringParenCasts(cxxBoolLiteral())),
                              hasAnyArgument(ignoringParenCasts(integerLiteral())),
                              hasAnyArgument(ignoringParenCasts(stringLiteral())),
                              hasAnyArgument(ignoringParenCasts(characterLiteral())),
                              hasAnyArgument(ignoringParenCasts(floatLiteral())))))
                  .bind("caller");

  // LAC is the callback that will run when the ASTMatcher finds the pattern
  // above.
  Finder.addMatcher(CallSiteMatcher, &LACHandler);
}