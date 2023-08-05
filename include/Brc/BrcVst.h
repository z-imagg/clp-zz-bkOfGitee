#ifndef BrcVst_H
#define BrcVst_H


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
class BrcVst
        : public RecursiveASTVisitor<BrcVst> {
public:
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit BrcVst(CompilerInstance &CI, const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, SourceManager& SM, LangOptions &_langOptions)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter_ptr(rewriter_ptr),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }


    void letLRBraceWrapRangeAftBf(SourceLocation B, SourceLocation E, const char* whoInserted="");
    void letLRBraceWrapStmtBfAfTk(Stmt *stmt, const char* whoInserted="");

    virtual bool TraverseIfStmt(IfStmt *ifStmt);

    virtual bool TraverseWhileStmt(WhileStmt *whileStmt);

    virtual bool TraverseForStmt(ForStmt *forStmt);

    virtual bool TraverseSwitchStmt(SwitchStmt *switchStmt);

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