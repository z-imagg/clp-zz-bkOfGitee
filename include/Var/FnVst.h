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
/* c++构造函数声明和实现 在不在同一个源文件 中 对于 TraverseCXXMethodDecl 的差异
1. c++构造函数声明和实现分开放时,TraverseCXXMethodDecl并不会遍历这样的构造函数实现,因此FnVst需要明确写出TraverseCXXConstructorDecl
     c++构造函数声明和实现分开放==’声明在.h、实现在.cxx‘
2. c++构造函数声明和实现放在同一个源文件中时,TraverseCXXMethodDecl并会遍历这样的构造函数实现
     c++构造函数声明和实现放在同一个源文件中 == ‘不论实现是否写在类声明中, 只要 声明在和实现都在同一个x.h 或 声明和实现都在同一个x.cpp’
     */
    bool TraverseCXXConstructorDecl(CXXConstructorDecl* cxxCnstrDecl);//CXXConstructorDecl是一种CXXMethodDecl
    bool TraverseCXXMethodDecl(CXXMethodDecl* cxxMthD);
    bool TraverseCXXConversionDecl(CXXConversionDecl * cxxCnvDecl);
    bool TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl);//CXXDestructorDecl是一种CXXMethodDecl
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