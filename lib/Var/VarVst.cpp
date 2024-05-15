#include "Var/VarVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include <vector>

#include <fmt/core.h>

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>
#include <fmt/core.h>


using namespace llvm;
using namespace clang;

void printDeclStmtTree(DeclStmt *declStmt, int depth) {
    // 获取当前节点的声明
    Decl *decl = nullptr;
    if (auto *singleDecl = declStmt->getSingleDecl()) {
        decl = singleDecl;

    } else  {
        // 如果是一组声明，则选择第一个声明
        decl=* (declStmt->getDeclGroup().begin());
    }

    // 如果找到声明，则打印其名称
    if (decl) {
//        decl->getKind()
        Decl::Kind declKind = decl->getKind();
        std::string  msg=fmt::format("depth={}, kind={}, DeclKindName={}\n",depth,(int)declKind,decl->getDeclKindName());
//        llvm::errs() << msg ;
        std::cout<<msg;
    }

//    Stmt *body = decl->getBody();
    // 打印当前节点的子节点
    for (auto *child : declStmt->children()) {
        if (auto *childDeclStmt = llvm::dyn_cast<DeclStmt>(child)) {
            printDeclStmtTree(childDeclStmt, depth + 1);
        } else {

            // 如果子节点不是 DeclStmt，则打印其名称
            Stmt::StmtClass childStmtClass = child->getStmtClass();
            std::string  msg=fmt::format("depth={}, StmtClass={}, StmtClassName={}\n",depth+1,(int)childStmtClass,child->getStmtClassName());
//            llvm::errs() << msg  ;
            std::cout<<msg;
        }
    }
}

bool VarVst::TraverseDeclStmt(DeclStmt* declStmt){
    std::cout<<"\n";
//    printDeclStmtTree(declStmt,0);
    Stmt::StmtClass stmtClass = declStmt->getStmtClass();
    const char *stmtClassName = declStmt->getStmtClassName();

    bool isSingleDecl = declStmt->isSingleDecl();
    Decl *p_singleDecl = declStmt->getSingleDecl();
    Decl &singleDecl = *p_singleDecl;
    Stmt *body = singleDecl.getBody();
    AttrVec &attrVec = singleDecl.getAttrs();
    size_t vecCnt = attrVec.size();

    if (ValueDecl *valueDecl = dyn_cast<ValueDecl>(&singleDecl)) {
        const QualType &qualType = valueDecl->getType();
        clang::Type::TypeClass typeClass = qualType->getTypeClass();
        const clang::Type *typePtr = qualType.getTypePtr();
        const char *typeClassName = typePtr->getTypeClassName();
        //比如  ' qualType.getTypePtr()->getTypeClassName()== "Builtin" 或 基本类型的"Typedef" ' 即  'clang::Type::Builtin == qualType->getTypeClass()'
        bool typeClassEqBuiltin = clang::Type::Builtin == typeClass;
        bool isBuiltinType = qualType->isBuiltinType();
        if(typeClassEqBuiltin){
            //断言 typeClassEqBuiltin==isBuiltinType
            assert(isBuiltinType);
            return true;
        }


        bool typeClassEqRecord = clang::Type::Record == typeClass;
        bool isObjectType = qualType->isObjectType();
        if(typeClassEqRecord){
            //断言 typeClassEqRecord==isObjectType
            assert(isObjectType);
            //TODO 【执行业务内容】 向threadLocal记录发生一次 :  栈区变量声明 其类型为typeClassName
        }

//        bool isTrivialType = qualType.isTrivialType(*Ctx);
        int isClassType = qualType->isClassType();
//        int isStructuralType = qualType->isStructuralType();
        int isFloatingType = qualType->isFloatingType();
        const std::string &typeName = qualType.getAsString();
        std::string  msg=fmt::format("typeName='{}',typeClass={},typeClassName={},typeClassEqBuiltin={},isObjectType={},isBuiltinType={},isClassType={},isFloatingType={}\n", typeName, (int)typeClass, typeClassName, typeClassEqBuiltin, isObjectType, isBuiltinType, isClassType, isFloatingType);
        std::cout<<msg;
        
        
    }
    Decl::Kind singleDeclKind = singleDecl.getKind();

//    SourceLocation beginLoc,endLoc;
//    beginLoc = declStmt->getBeginLoc();
//    endLoc = declStmt->getEndLoc();
    //构造人类可读开始位置、结束位置、插入者 注释文本
//    std::string hmTxtCmnt_whoInsrt_BE;
//    Util::BE_Loc_HumanText(SM, beginLoc, endLoc, "test_VisitDeclStmt", hmTxtCmnt_whoInsrt_BE);
//    std::cout<<hmTxtCmnt_whoInsrt_BE<<"\n";

//    Util::printDecl(*Ctx,CI,"tag1","title1",&singleDecl,true);
    Util::printStmt(*Ctx,CI,"tag1","title1",declStmt,true);
    return true;
}

//TODO 【块离开时， 执行业务内容】 遍历每个k: 直接在该块内声明的栈区变量k，找到变量k的类型名Tk，向threadLocal记录 释放了一个类型为Tk的变量