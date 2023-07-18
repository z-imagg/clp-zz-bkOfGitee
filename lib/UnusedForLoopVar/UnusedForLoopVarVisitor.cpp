#include "UnusedForLoopVar.h"

#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

std::string NameToMatch("[U|u][N|n][U|u][S|s][E|e][D|d]");

//-----------------------------------------------------------------------------
// UnusedForLoopVarVisitor implementation
//-----------------------------------------------------------------------------
bool UnusedForLoopVarVisitor::TraverseForStmt(ForStmt *S) {
  // Get DeclStmt corresponding to init in 'for (init;cond;inc)'
  auto InitStmt = dyn_cast<DeclStmt>(S->getInit());

  // Loop over all variables declared in the init S and add them to
  // VarsDeclaredInLoopInitStmt. Note that we care only about indices that _are
  // declared_ in the init statement, e.g. loops like this one:
  //    for (int i = 0; cond; inc)
  // We ignore loop indices declared outside the init statement, e.g. in loops
  // like the following:
  //    int i;
  //    for (i = 0; cond; inc)
  // The former are identified by checking the type of the init nodes is
  // VarDecl.
  for (auto *InitVar : InitStmt->decls()) {
    VarDecl const *InitVarDecl = nullptr;
    InitVarDecl = dyn_cast<VarDecl>(InitVar);
    if (InitVarDecl && !llvm::Regex(NameToMatch).match(InitVarDecl->getName()))
      VarsDeclaredInLoopInitStmt.push_back(InitVarDecl);
  }

  // If there are no variables declared in the init statement, return true to
  // continue traversing the AST.
  if (VarsDeclaredInLoopInitStmt.empty())
    return true;

  // Traverse, recursively, the body of the loop and record every use of the
  // loop indices
  auto Ret = RecursiveASTVisitor::TraverseStmt(S->getBody());

  // For every loop variable declaration, LoopVarDecl, check whether the
  // corresponding variable is used?
  for (auto LoopVarDecl : VarsDeclaredInLoopInitStmt) {
    if (UsedVars.erase(LoopVarDecl) == 0) {
      DiagnosticsEngine &DE = Ctx->getDiagnostics();
      unsigned DiagID = DE.getCustomDiagID(
              DiagnosticsEngine::Warning,
              "(Recursive AST Visitor) regular for-loop variable not used");
      auto DB = DE.Report(LoopVarDecl->getLocation(), DiagID);
      DB.AddSourceRange(
              clang::CharSourceRange::getCharRange(LoopVarDecl->getSourceRange()));
    }
  }

  // Clean-up before visiting the next loop
  VarsDeclaredInLoopInitStmt.clear();
  UsedVars.clear();

  // Return whether the visitation was terminated early
  return Ret;
}

bool UnusedForLoopVarVisitor::VisitDeclRefExpr(DeclRefExpr const *Stmt) {
  // Get the declaration of the variable being referenced
  auto ReferencedVarDecl = dyn_cast<VarDecl>(Stmt->getDecl());

  // TODO - when do we not hit this?
  if (!ReferencedVarDecl)
    return true;

  // If this is a variable declared in the lopp's init statement, record this
  // for later.
  if (std::find(VarsDeclaredInLoopInitStmt.begin(),
                VarsDeclaredInLoopInitStmt.end(),
                ReferencedVarDecl) != VarsDeclaredInLoopInitStmt.end())
    UsedVars.insert(ReferencedVarDecl);

  return true;
}

