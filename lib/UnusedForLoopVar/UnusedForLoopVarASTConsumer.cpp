#include "UnusedForLoopVar/UnusedForLoopVarASTConsumer.h"

#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

extern std::string NameToMatch;




namespace {

    AST_MATCHER_P(NamedDecl, matchesAnyListedName, std::string, Name) {
    return llvm::Regex(Name).match(Node.getName());
}
} // namespace
//-----------------------------------------------------------------------------
// UnusedForLoopVarASTConsumer - implementation
//-----------------------------------------------------------------------------
UnusedForLoopVarASTConsumer::UnusedForLoopVarASTConsumer(ASTContext &CTX,
                                                         SourceManager &InSM)
        : SM(InSM), UFLVVisitor(&CTX) {
  // ASTMatcher for a range for-loop
  auto LoopVar = varDecl(unless(matchesAnyListedName(NameToMatch)));
  StatementMatcher For =
          cxxForRangeStmt(hasLoopVariable(LoopVar.bind("LoopVar")))
                  .bind("RangeForLoop");

  Matcher.addMatcher(For, &UFLVMatcher);

  // ASTMatcher for a regular for-loop
  const auto LoopVarsInit =
          declStmt(unless(forEach(varDecl(matchesAnyListedName(NameToMatch)))))
                  .bind("LoopVarRegularFor");
  auto ForStmt = forStmt(hasLoopInit(LoopVarsInit)).bind("RegularForLoop");

  Matcher.addMatcher(ForStmt, &UFLVMatcher);

  // TODO: clang-tidy uses `this`
}
