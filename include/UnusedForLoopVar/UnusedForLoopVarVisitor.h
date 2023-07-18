
#ifndef CLANG_TUTOR_UNUSEDFORLOOPVARVISITOR_H
#define CLANG_TUTOR_UNUSEDFORLOOPVARVISITOR_H


#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"


//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class UnusedForLoopVarVisitor
        : public clang::RecursiveASTVisitor<UnusedForLoopVarVisitor> {
public:
    explicit UnusedForLoopVarVisitor(clang::ASTContext *Ctx) : Ctx(Ctx) {}
    // TraverseFoo(Foo *x) methods are documented in
    // clang/include/clang/AST/RecursiveASTVisitor.h (above the declaration of
    // the `RecursiveASTVisitor` class)
    bool TraverseForStmt(clang::ForStmt *S);
    // VisitFoo(Foo *x) methods are documented in
    // clang/include/clang/AST/RecursiveASTVisitor.h (above the declaration of
    // the `RecursiveASTVisitor` class)
    bool VisitDeclRefExpr(clang::DeclRefExpr const *S);

private:
    clang::ASTContext *Ctx;

    llvm::SmallVector<clang::VarDecl const *, 4> VarsDeclaredInLoopInitStmt;
    llvm::SmallSet<clang::VarDecl const *, 4> UsedVars;
};


#endif //CLANG_TUTOR_UNUSEDFORLOOPVARVISITOR_H
