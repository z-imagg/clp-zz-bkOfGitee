#ifndef X__FindTClkCall_ReadOnly_Visitor_H
#define X__FindTClkCall_ReadOnly_Visitor_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"

using namespace llvm;
using namespace clang;

/**
 * 只读CallExpr遍历器,  找时钟调用语句
 */
class FindTClkCall_ReadOnly_Visitor
        : public RecursiveASTVisitor<FindTClkCall_ReadOnly_Visitor> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit FindTClkCall_ReadOnly_Visitor(SourceManager &_SM, LangOptions &_langOptions,ASTContext *_Ctx)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    :
    SM(_SM),
    langOptions(_langOptions),
    Ctx(_Ctx),
    curMainFileHas_TCTickCall(false)
    {

    }

    virtual bool VisitCallExpr(CallExpr *callExpr);



public:
    SourceManager &SM;
    ASTContext *Ctx;
    LangOptions &langOptions;
    bool curMainFileHas_TCTickCall;

};


#endif