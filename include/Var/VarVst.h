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




//    virtual bool VisitDeclStmt(DeclStmt* declStmt);
    virtual bool TraverseDeclStmt(DeclStmt* declStmt);
    bool process_singleDecl(const Decl *p_singleDecl);
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