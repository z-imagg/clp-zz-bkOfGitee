#ifndef FndCTkClROVst_H
#define FndCTkClROVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"

using namespace llvm;
using namespace clang;

/**
 * 只读CallExpr遍历器,  找时钟调用语句
 */
class FndCTkClROVst
        : public RecursiveASTVisitor<FndCTkClROVst> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit FndCTkClROVst(CompilerInstance &_CI,SourceManager &_SM, LangOptions &_langOptions, ASTContext *_Ctx)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    :
    CI(_CI),
    SM(_SM),
    langOptions(_langOptions),
    Ctx(_Ctx),
    curMainFileHas_TCTkCall(false)
    {

    }

    virtual bool VisitCallExpr(CallExpr *callExpr);



public:
    CompilerInstance &CI;
    SourceManager &SM;
    ASTContext *Ctx;
    LangOptions &langOptions;
    bool curMainFileHas_TCTkCall;

};


#endif