#include "UnusedForLoopVar.h"

#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;


//-----------------------------------------------------------------------------
// UnusedForLoopVarMatcher - implementation
//-----------------------------------------------------------------------------
void UnusedForLoopVarMatcher::run(const MatchFinder::MatchResult &Result) {
  // ASTContext is used to retrieve the source location
  ASTContext *Ctx = Result.Context;

  if (Result.Nodes.getNodeAs<clang::ForStmt>("RegularForLoop"))
    return runRegularForLoop(Result, Ctx);

  if (Result.Nodes.getNodeAs<clang::CXXForRangeStmt>("RangeForLoop"))
    return runRangeForLoop(Result, Ctx);
}

void UnusedForLoopVarMatcher::runRegularForLoop(
        const MatchFinder::MatchResult &Result, ASTContext *Ctx) {
  // Retrieve the matched nodes
  const ForStmt *RegularForLoop =
          Result.Nodes.getNodeAs<clang::ForStmt>("RegularForLoop");
  const DeclStmt *LoopVarRegularFor =
          Result.Nodes.getNodeAs<clang::DeclStmt>("LoopVarRegularFor");

  // Don't follow #include files
  if (RegularForLoop &&
      !Ctx->getSourceManager().isInMainFile(RegularForLoop->getForLoc()))
    return;

  // Loop over all variables declared in the init statement of the loop
  for (auto *LoopVar : LoopVarRegularFor->decls()) {
    // If LoopVar _is used_ there's nothing to report
    if (LoopVar->isUsed(true))
      continue;

    // LoopVar is not used -> create a warning
    DiagnosticsEngine &DE = Ctx->getDiagnostics();
    unsigned DiagID =
            DE.getCustomDiagID(DiagnosticsEngine::Warning,
                               "(AST Matcher) regular for-loop variable not used");
    auto DB = DE.Report(LoopVar->getLocation(), DiagID);
    DB.AddSourceRange(
            clang::CharSourceRange::getCharRange(LoopVar->getSourceRange()));
  }
}

void UnusedForLoopVarMatcher::runRangeForLoop(
        const MatchFinder::MatchResult &Result, ASTContext *Ctx) {
  // Retrieve the matched nodes
  const VarDecl *LoopVar = Result.Nodes.getNodeAs<clang::VarDecl>("LoopVar");
  const CXXForRangeStmt *RangeForLoop =
          Result.Nodes.getNodeAs<clang::CXXForRangeStmt>("RangeForLoop");

  // Don't follow #include files: range for-loop
  if (RangeForLoop &&
      !Ctx->getSourceManager().isInMainFile(RangeForLoop->getForLoc()))
    return;

  // If LoopVar _is used_ there's nothing to report. Note that in the following
  // case, `j` will be flagged as _used_ and `i` will be flagged as _not_used_.
  // ```cpp
  // for (int j = 0, i = 0; j < 20; j++) { }
  // ```
  if (LoopVar->isUsed(true))
    return;

  // LoopVar is not used -> create a warning
  DiagnosticsEngine &DE = Ctx->getDiagnostics();
  unsigned DiagID =
          DE.getCustomDiagID(DiagnosticsEngine::Warning,
                             "(AST Matcher) range for-loop variable not used");
  auto DB = DE.Report(LoopVar->getLocation(), DiagID);
  DB.AddSourceRange(
          clang::CharSourceRange::getCharRange(LoopVar->getSourceRange()));
}

