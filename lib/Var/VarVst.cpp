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

    bool isStructType;
    std::string typeName;
    QualType qualType;

    bool isSingleDecl = declStmt->isSingleDecl();
    if(isSingleDecl){
        //单声明（单变量声明、单函数声明、单x声明）
        Decl *p_singleDecl = declStmt->getSingleDecl();
        bool result= this->process_singleDecl(p_singleDecl,isStructType,typeName,qualType);
        clang::Type::TypeClass  typeClass = qualType->getTypeClass();
        return result;
    }else{
        //多声明（多变量声明、多函数声明、多x声明）
        //只看第1个声明
        Decl * decl0=* (declStmt->getDeclGroup().begin());
        this->process_singleDecl(decl0,isStructType,typeName,qualType);
        clang::Type::TypeClass  typeClass = qualType->getTypeClass();

        const DeclGroupRef &declGroup = declStmt->getDeclGroup();
        //遍历每一个声明
        std::for_each(declGroup.begin(),declGroup.end(),[this,isStructType,typeName,typeClass](const Decl* decl_k){

            bool isStructType_k;
            std::string typeName_k;
            QualType qualType_k;
            this->process_singleDecl(decl_k,isStructType_k,typeName_k,qualType_k);
            const char *typeClassName_k = qualType_k.getTypePtr()->getTypeClassName();
            clang::Type::TypeClass  typeClass_k = qualType_k->getTypeClass();
            if(typeClass!=clang::Type::Pointer &&
            typeClass_k!=clang::Type::Pointer) //等价于 typeClassName_k!="Pointer"
            {
                //断言第k个声明和第0个声明的类型一致
                MyAssert(isStructType_k==isStructType && typeName_k==typeName,"[AssertErr]NotFit:isStructType_k==isStructType && typeName_k==typeName" )
            }
        });
        return true;
    }

}


bool VarVst::process_singleDecl(const Decl *p_singleDecl,bool& isStructType,std::string &typeName,QualType &qualType){


    if (const ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(p_singleDecl)) {
        qualType = valueDecl->getType();
        clang::Type::TypeClass typeClass = qualType->getTypeClass();
        const clang::Type *typePtr = qualType.getTypePtr();
        const char *typeClassName = typePtr->getTypeClassName();
        bool isBuiltinType = qualType->isBuiltinType();

        bool typeClassEqRecord = clang::Type::Record == typeClass;
        bool typeClassEqElaborated = clang::Type::Elaborated == typeClass;

        bool isPointerType=qualType->isPointerType();
        typeName = qualType.getAsString();

        std::string  msg=fmt::format("typeName='{}',typeClass={},typeClassName={},isBuiltinType={}\n", typeName, (int)typeClass, typeClassName, isBuiltinType);
        std::cout<<msg;

        if(isBuiltinType){
            isStructType=false;
            std::cout<<"[跳过]isBuiltinType==true\n";
            return true;
        }
        if(isPointerType){
            isStructType=false;
            std::cout<<"[跳过]isPointerType==true\n";
            return true;
        }

        isStructType=(typeClassEqRecord||typeClassEqElaborated);
        MyAssert(isStructType,"[AssertErr]NotFit:typeClassEqRecord||typeClassEqElaborated");

        //TODO 【执行业务内容】 向threadLocal记录发生一次 :  栈区变量声明 其类型为typeClassName

    }

    return true;
}

//TODO 【块离开时， 执行业务内容】 遍历每个k: 直接在该块内声明的栈区变量k，找到变量k的类型名Tk，向threadLocal记录 释放了一个类型为Tk的变量