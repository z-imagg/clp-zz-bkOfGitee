#include "CodeRefactor.h"

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

//-----------------------------------------------------------------------------
// CodeRefactorMatcher - implementation
//-----------------------------------------------------------------------------
void CodeRefactorMatcher::run(const MatchFinder::MatchResult &Result) {
  const MemberExpr *MemberAccess =
          Result.Nodes.getNodeAs<clang::MemberExpr>("MemberAccess");

  if (MemberAccess) {
    SourceRange CallExprSrcRange = MemberAccess->getMemberLoc();
    CodeRefactorRewriter.ReplaceText(CallExprSrcRange, NewName);
  }

  const NamedDecl *MemberDecl =
          Result.Nodes.getNodeAs<clang::NamedDecl>("MemberDecl");

  if (MemberDecl) {
    SourceRange MemberDeclSrcRange = MemberDecl->getLocation();
    CodeRefactorRewriter.ReplaceText(
            CharSourceRange::getTokenRange(MemberDeclSrcRange), NewName);
  }
}

void CodeRefactorMatcher::onEndOfTranslationUnit() {
  // Output to stdout
  CodeRefactorRewriter
          .getEditBuffer(CodeRefactorRewriter.getSourceMgr().getMainFileID())
          .write(llvm::outs());
}
