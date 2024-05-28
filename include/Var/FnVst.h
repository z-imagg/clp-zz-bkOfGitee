#ifndef FnVst_H
#define FnVst_H


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
class FnVst
        : public RecursiveASTVisitor<FnVst> {
public:
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit FnVst(CompilerInstance &CI, const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, SourceManager& SM, LangOptions &_langOptions)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter_ptr(rewriter_ptr),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }

    bool insert_init__After_FnBdLBrc( bool useCXX,LocId fnBdLBrcLocId,std::string funcName, SourceLocation funcBodyLBraceLoc , SourceLocation funcBodyRBraceLoc );
    virtual bool TraverseFunctionDecl(FunctionDecl* funcDecl);
    //bool TraverseCXXConstructorDecl(CXXConstructorDecl* cxxCnstrDecl);//CXXConstructorDecl是一种CXXMethodDecl
    bool TraverseCXXMethodDecl(CXXMethodDecl* cxxMethDecl);
    bool TraverseCXXConversionDecl(CXXConversionDecl * cxxCnvDecl);
    //bool TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl);//CXXDestructorDecl是一种CXXMethodDecl
    bool I__TraverseCXXMethodDecl(CXXMethodDecl* cxxMethDecl,const char* who);
    bool TraverseLambdaExpr(LambdaExpr *lambdaExpr);
    bool _Traverse_Func(
//            bool funcIsStatic,
//            bool funcIsInline,
            QualType funcReturnType,
            bool isaCXXConstructorDecl,
            Stmt *endStmtOfFuncBody,
            SourceLocation funcBodyLBraceLoc,
            SourceLocation funcBodyRBraceLoc,
            LocId funcBodyLBraceLocId,
            LocId funcBodyRBraceLocId,
            CompoundStmt* compoundStmt,
            std::string funcName
    );





public:
    const std::shared_ptr<Rewriter> mRewriter_ptr;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

    //函数体左花括号紧后
    std::unordered_set<LocId,LocId> fnBdLBrcLocIdSet;
};


#endif