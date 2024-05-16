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
#include <iostream>
#include <clang/AST/ParentMapContext.h>

#include "base/MyAssert.h"

using namespace llvm;
using namespace clang;


bool VarVst::TraverseDeclStmt(DeclStmt* declStmt){
    std::cout<<"\n";

//    Util::printDecl(*Ctx,CI,"tag1","title1",&singleDecl,true);
    Util::printStmt(*Ctx,CI,"tag1","title1",declStmt,true);

    bool isSingleDecl = declStmt->isSingleDecl();
    if(isSingleDecl){
        //单声明（单变量声明、单函数声明、单x声明）
        Decl *p_singleDecl = declStmt->getSingleDecl();
        return this->process_singleDecl(p_singleDecl);
    }else{
        //多声明（多变量声明、多函数声明、多x声明）
//        Decl * decl=* (declStmt->getDeclGroup().begin());
        const DeclGroupRef &declGroup = declStmt->getDeclGroup();
        std::for_each(declGroup.begin(),declGroup.end(),[this](const Decl* declK){
            this->process_singleDecl(declK);
        });
        return true;
    }

}


bool VarVst::process_singleDecl(const Decl *p_singleDecl){


    if (const ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(p_singleDecl)) {
        const QualType &qualType = valueDecl->getType();
        clang::Type::TypeClass typeClass = qualType->getTypeClass();
        const clang::Type *typePtr = qualType.getTypePtr();
        const char *typeClassName = typePtr->getTypeClassName();
        //比如  ' qualType.getTypePtr()->getTypeClassName()== "Builtin" 或 基本类型的"Typedef" ' 即  'clang::Type::Builtin == qualType->getTypeClass()'
        bool typeClassEqBuiltin = clang::Type::Builtin == typeClass;
        bool isBuiltinType = qualType->isBuiltinType();


        bool typeClassEqRecord = clang::Type::Record == typeClass;
        bool typeClassEqElaborated = clang::Type::Elaborated == typeClass;
        bool isObjectType = qualType->isObjectType();



//        bool isTrivialType = qualType.isTrivialType(*Ctx);
        bool isClassType = qualType->isClassType();
        bool isPointerType=qualType->isPointerType();
//        int isStructuralType = qualType->isStructuralType();
        bool isFloatingType = qualType->isFloatingType();
        const std::string &typeName = qualType.getAsString();

        std::string  msg=fmt::format("typeName='{}',typeClass={},typeClassName={},typeClassEqBuiltin={},isObjectType={},isBuiltinType={},isClassType={},isFloatingType={}\n", typeName, (int)typeClass, typeClassName, typeClassEqBuiltin, isObjectType, isBuiltinType, isClassType, isFloatingType);
        std::cout<<msg;

        //断言 typeClassEqBuiltin==isBuiltinType
//        MyAssert(typeClassEqBuiltin == isBuiltinType,"[AssertErr]NotFit:typeClassEqBuiltin==isBuiltinType"); //失败举例 typeName=='DOUBLE_typedef' && isBuiltinType=true
        //断言 typeClassEqRecord==isObjectType
//        MyAssert(typeClassEqRecord == isObjectType,"[AssertErr]NotFit:typeClassEqRecord==isObjectType"); //失败举例 typeClassEqRecord=='Pointer' && isObjectType==true


        if(isBuiltinType){
            std::cout<<"[跳过]isBuiltinType==true\n";
            return true;
        }
        if(isPointerType){
            std::cout<<"[跳过]isPointerType==true\n";
            return true;
        }

        MyAssert(typeClassEqRecord||typeClassEqElaborated,"[AssertErr]NotFit:typeClassEqRecord||typeClassEqElaborated");

        //TODO 【执行业务内容】 向threadLocal记录发生一次 :  栈区变量声明 其类型为typeClassName

    }
    Decl::Kind singleDeclKind = p_singleDecl->getKind();

//    SourceLocation beginLoc,endLoc;
//    beginLoc = declStmt->getBeginLoc();
//    endLoc = declStmt->getEndLoc();
    //构造人类可读开始位置、结束位置、插入者 注释文本
//    std::string hmTxtCmnt_whoInsrt_BE;
//    Util::BE_Loc_HumanText(SM, beginLoc, endLoc, "test_VisitDeclStmt", hmTxtCmnt_whoInsrt_BE);
//    std::cout<<hmTxtCmnt_whoInsrt_BE<<"\n";

    return true;
}

//TODO 【块离开时， 执行业务内容】 遍历每个k: 直接在该块内声明的栈区变量k，找到变量k的类型名Tk，向threadLocal记录 释放了一个类型为Tk的变量