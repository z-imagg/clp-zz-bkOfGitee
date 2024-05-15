#ifndef VarVst_H
#define VarVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include <unordered_set>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "base/LocId.h"

using namespace llvm;
using namespace clang;

/**
 * 插入花括号Visitor
 */
class VarVst
        : public RecursiveASTVisitor<VarVst> {
public:
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit VarVst(CompilerInstance &CI, const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, SourceManager& SM, LangOptions &_langOptions)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter_ptr(rewriter_ptr),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }



    void letLRBraceWrapRangeAftBf(SourceLocation B, SourceLocation E, const char* whoInserted="");
    void letLRBraceWrapStmtBfAfTk(Stmt *stmt, const char* whoInserted="");

    //   RecursiveASTVisitor_H="/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h"
    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(CompoundStmt'  $RecursiveASTVisitor_H
//    virtual bool TraverseCompoundStmt(CompoundStmt *compoundStmt );

    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(IfStmt'  $RecursiveASTVisitor_H
    virtual bool TraverseIfStmt(IfStmt *ifStmt);

    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(WhileStmt'  $RecursiveASTVisitor_H
    virtual bool TraverseWhileStmt(WhileStmt *whileStmt);

    //forEach和for很相似
    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(ForStmt'  $RecursiveASTVisitor_H
    virtual bool TraverseForStmt(ForStmt *forStmt);
    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(CXXTryStmt'  $RecursiveASTVisitor_H
//    virtual bool TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt);
    // 该函数默认实现: grep -A3  'DEF_TRAVERSE_STMT(CXXCatchStmt'  $RecursiveASTVisitor_H
//    virtual bool TraverseCXXCatchStmt(CXXCatchStmt *cxxCatchStmt);
    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(DoStmt'  $RecursiveASTVisitor_H
//    virtual bool TraverseDoStmt(DoStmt *doStmt);
    // 该函数默认实现: grep -A10 'DEF_TRAVERSE_STMT(CXXForRangeStmt'  $RecursiveASTVisitor_H
    virtual bool TraverseCXXForRangeStmt(CXXForRangeStmt *forRangeStmt);

    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(SwitchStmt'  $RecursiveASTVisitor_H
    virtual bool TraverseSwitchStmt(SwitchStmt *switchStmt);
    // 该函数默认空实现: grep 'DEF_TRAVERSE_STMT(CaseStmt'  $RecursiveASTVisitor_H
//    virtual bool TraverseCaseStmt(CaseStmt *caseStmt);

    // 该函数默认实现: grep -A6 'DEF_TRAVERSE_DECL(FunctionDecl'  $RecursiveASTVisitor_H
//    virtual bool TraverseFunctionDecl(FunctionDecl* funcDecl);
    /** CXXConstructorDecl默认遍历器: grep -A6 'DEF_TRAVERSE_DECL(CXXConstructorDecl'  $RecursiveASTVisitor_H
     bool TraverseCXXConstructorDecl(CXXConstructorDecl* cxxCnstrDecl){
     ShouldVisitChildren = false;//不访问子节点
     ReturnValue = TraverseFunctionHelper(D);
    }
     */
//    virtual bool TraverseCXXConstructorDecl(CXXConstructorDecl* cxxCnstrDecl);
    // 该函数默认实现: grep -A38 'DEF_TRAVERSE_STMT(LambdaExpr'  $RecursiveASTVisitor_H
//    virtual bool TraverseLambdaExpr(LambdaExpr *lambdaExpr);
    // 该函数默认实现: grep  -A6   'DEF_TRAVERSE_DECL(CXXDestructorDecl'  $RecursiveASTVisitor_H
//    virtual bool TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl);

//    virtual bool VisitDeclStmt(DeclStmt* declStmt);
    virtual bool TraverseDeclStmt(DeclStmt* declStmt);
    /**
'#define STMT'下1行有'bool Traverse##CLASS'下1行'#include "clang/AST/StmtNodes.inc"'

python /app/bash-simplify/pyFileReFindAllDotAsAll.py  '#define STMT(?:.*\n){1}  bool Traverse##CLASS(?:.*\n)#include "clang/AST/StmtNodes.inc"\n'  "$RecursiveASTVisitor_H"

文件/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h 匹配行号 行372～行375
 #define STMT(CLASS, PARENT) \
  bool Traverse##CLASS(CLASS *S, DataRecursionQueue *Queue = nullptr);
#include "clang/AST/StmtNodes.inc"


'#define STMT'下6行有'bool Visit##CLASS'下1行'#include "clang/AST/StmtNodes.inc"'

python /app/bash-simplify/pyFileReFindAllDotAsAll.py  '#define STMT(?:.*\n){6}  bool Visit##CLASS(?:.*\n){1}#include "clang/AST/StmtNodes.inc"\n'   "$RecursiveASTVisitor_H"

文件/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h 匹配行号 行380～行388
 #define STMT(CLASS, PARENT)                                                    \
  bool WalkUpFrom##CLASS(CLASS *S) {                                           \
    TRY_TO(WalkUpFrom##PARENT(S));                                             \
    TRY_TO(Visit##CLASS(S));                                                   \
    return true;                                                               \
  }                                                                            \
  bool Visit##CLASS(CLASS *S) { return true; }
#include "clang/AST/StmtNodes.inc"
*/




public:
    const std::shared_ptr<Rewriter> mRewriter_ptr;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

    //一个位置若是插入了花括号，则表明此位置不需要再次插入花括号了。
    std::unordered_set<LocId,LocId> LBraceLocIdSet;

};


#endif