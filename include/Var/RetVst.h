#ifndef RetVst_H
#define RetVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include <unordered_set>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "base/LocId.h"

using namespace llvm;
using namespace clang;

/**
 * 先以 clang插件clang-voidFnEndInsertRet 确保  每个void函数末尾都有return语句
 * 再执行本插件
 */
class RetVst
        : public RecursiveASTVisitor<RetVst> {
public:
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit RetVst(CompilerInstance &CI, const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, SourceManager& SM, LangOptions &_langOptions)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter_ptr(rewriter_ptr),
    Ctx(Ctx),CtxRef(*Ctx),
    CI(CI),
    SM(SM)
    {

    }

    bool insert_destroy__Before_fnRet(LocId retBgnLocId, SourceLocation retBgnLoc  );

    bool TraverseReturnStmt(ReturnStmt *returnStmt);


public:
    const std::shared_ptr<Rewriter> mRewriter_ptr;

public:
    ASTContext *Ctx;
    ASTContext &CtxRef;
    CompilerInstance& CI;
    SourceManager& SM;

    //return紧前
    std::unordered_set<LocId,LocId> retBgnLocIdSet;
};


#endif