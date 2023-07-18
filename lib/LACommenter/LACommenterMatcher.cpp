
#include "LACommenter/LACommenterMatcher.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace ast_matchers;

//-----------------------------------------------------------------------------
// LACommenter - implementation
//-----------------------------------------------------------------------------
void LACommenterMatcher::run(const MatchFinder::MatchResult &Result) {
  // ASTContext is used to retrieve the source location
  ASTContext *Ctx = Result.Context;

  // Callee and caller are accessed via .bind("callee") and .bind("caller"),
  // respectively, from the ASTMatcher
  const FunctionDecl *CalleeDecl =
          Result.Nodes.getNodeAs<clang::FunctionDecl>("callee");
  const CallExpr *TheCall = Result.Nodes.getNodeAs<clang::CallExpr>("caller");

  // Basic sanity checking
  assert(TheCall && CalleeDecl &&
         "The matcher matched, so callee and caller should be non-null");

  // No arguments means there's nothing to comment
  if (CalleeDecl->parameters().empty())
    return;

  // Get the arguments
  Expr const *const *Args = TheCall->getArgs();
  size_t NumArgs = TheCall->getNumArgs();

  // If this is a call to an overloaded operator (e.g. `+`), then the first
  // parameter is the object itself (i.e. `this` pointer). Skip it.
  if (isa<CXXOperatorCallExpr>(TheCall)) {
    Args++;
    NumArgs--;
  }

  // For each argument match it with the callee parameter. If it is an integer,
  // float, boolean, character or string literal insert a comment.
  for (unsigned Idx = 0; Idx < NumArgs; Idx++) {
    const Expr *AE = Args[Idx]->IgnoreParenCasts();

    if (!dyn_cast<IntegerLiteral>(AE) && !dyn_cast<CXXBoolLiteralExpr>(AE) &&
        !dyn_cast<FloatingLiteral>(AE) && !dyn_cast<StringLiteral>(AE) &&
        !dyn_cast<CharacterLiteral>(AE))
      continue;

    // Parameter declaration
    ParmVarDecl *ParamDecl = CalleeDecl->parameters()[Idx];

    // Source code locations (parameter and argument)
    FullSourceLoc ParamLocation = Ctx->getFullLoc(ParamDecl->getBeginLoc());
    FullSourceLoc ArgLoc = Ctx->getFullLoc(AE->getBeginLoc());

    if (ParamLocation.isValid() && !ParamDecl->getDeclName().isEmpty() &&
        EditedLocations.insert(ArgLoc).second)
      // Insert the comment immediately before the argument
      LACRewriter.InsertText(
              ArgLoc,
              (Twine("/*") + ParamDecl->getDeclName().getAsString() + "=*/").str());
  }
}

void LACommenterMatcher::onEndOfTranslationUnit() {
  // Replace in place
  // LACRewriter.overwriteChangedFiles();

  // Output to stdout
  LACRewriter.getEditBuffer(LACRewriter.getSourceMgr().getMainFileID())
          .write(llvm::outs());
}