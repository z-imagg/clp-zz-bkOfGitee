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
        //遍历每一个声明
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
        bool isBuiltinType = qualType->isBuiltinType();

        bool typeClassEqRecord = clang::Type::Record == typeClass;
        bool typeClassEqElaborated = clang::Type::Elaborated == typeClass;

        bool isPointerType=qualType->isPointerType();
        const std::string &typeName = qualType.getAsString();

        std::string  msg=fmt::format("typeName='{}',typeClass={},typeClassName={},typeClassEqBuiltin={},isObjectType={},isBuiltinType={},isClassType={},isFloatingType={}\n", typeName, (int)typeClass, typeClassName, typeClassEqBuiltin, isObjectType, isBuiltinType, isClassType, isFloatingType);
        std::cout<<msg;

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

    return true;
}

//TODO 【块离开时， 执行业务内容】 遍历每个k: 直接在该块内声明的栈区变量k，找到变量k的类型名Tk，向threadLocal记录 释放了一个类型为Tk的变量