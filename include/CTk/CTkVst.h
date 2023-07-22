#ifndef CTkVst_H
#define CTkVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CTkVst
        : public RecursiveASTVisitor<CTkVst> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit CTkVst(Rewriter &R, ASTContext *Ctx, CompilerInstance &CI, SourceManager& SM)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter(R),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }

    static const std::string funcName_TCTk ;//= "X__t_clock_tick";
    static const std::string IncludeStmt_TCTk ; // = "#include \"t_clock_tick.h\"\n";

    /**遍历语句
     *
     * @param stmt
     * @return
     */
    virtual bool VisitStmt(Stmt *stmt);
    virtual bool VisitCallExpr(CallExpr *callExpr);

    //下面4个形如 bool VisitZzz(clang:Zzz *Decl)  的方法:, 其中Zzz的完整列表叙述 从 本文件第27行开始.
    bool VisitCXXRecordDecl(CXXRecordDecl *Decl);
    bool VisitCXXMethodDecl(CXXMethodDecl* Decl);
    bool VisitFunctionDecl(FunctionDecl *Decl);
    bool VisitVarDecl(VarDecl *Decl);
    bool VisitFieldDecl(FieldDecl *Decl);


public:
    Rewriter mRewriter;

private:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

};


#endif