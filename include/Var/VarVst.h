#ifndef VarVst_H
#define VarVst_H


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




//    virtual bool VisitDeclStmt(DeclStmt* decl_k);
    virtual bool TraverseDeclStmt(DeclStmt* decl_k);
    bool process_singleDecl(const Decl *p_singleDecl,bool& isStructType,std::string &typeName,QualType &qualType);
    bool insertAfter_VarDecl(const std::string typeName,int varCnt,LocId varDeclLocId, SourceLocation varDeclEndLoc );




public:
    const std::shared_ptr<Rewriter> mRewriter_ptr;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

    //一个位置若是插入了花括号，则表明此位置不需要再次插入花括号了。
    std::unordered_set<LocId,LocId> VarDeclLocIdSet;

};


#endif