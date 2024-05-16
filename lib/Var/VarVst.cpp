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

bool VarVst::insertAfter_VarDecl(const std::string typeName,int varCnt,LocId varDeclLocId, SourceLocation varDeclEndLoc ){
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    //region 构造插入语句
    std::string cStr_inserted=fmt::format(
            "createVar(\"{}\", {})  /* 创建变量通知,  {} */ ;",
            typeName, varCnt, varDeclLocId.to_string()
    );
    llvm::StringRef strRef(cStr_inserted);
    //endregion

    bool insertResult=mRewriter_ptr->InsertTextAfterToken(varDeclEndLoc , strRef);


    //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
    VarDeclLocIdSet.insert(varDeclLocId);

    //写函数id描述行
//  funcIdDescSrv.write(funcLocId); // 把 funcLocId.to_csv_line() 牵涉的列们 都 发送到 funcIdDescWebService 去

    return insertResult;
}

bool VarVst::TraverseDeclStmt(DeclStmt* declStmt){
    std::cout<<"\n";

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
    Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);

    const SourceLocation declStmtBgnLoc = declStmt->getEndLoc();
    
//    Util::printDecl(*Ctx,CI,"tag1","title1",&singleDecl,true);
    Util::printStmt(*Ctx,CI,"tag1","title1",declStmt,true);

    bool likeStruct;
    std::string typeName;
    QualType qualType;
    int varCnt=0;

    Decl *singleDecl;
    bool result=false;

    bool isSingleDecl = declStmt->isSingleDecl();
    if(isSingleDecl){
        varCnt=1;
        //单声明（单变量声明、单函数声明、单x声明）
        singleDecl = declStmt->getSingleDecl();
    }else{
        //多声明（多变量声明、多函数声明、多x声明）
        const DeclGroupRef &dg = declStmt->getDeclGroup();
//        varCnt=std::distance(dg.begin(),dg.end());
        //只看第1个声明
        singleDecl=* (dg.begin());
    }

    // 多声明 result 依赖 第0个声明
    // 单声明 result 依赖 该声明
    result= this->process_singleDecl(singleDecl, likeStruct, typeName, qualType);
    clang::Type::TypeClass  typeClass = qualType->getTypeClass();

    if(isSingleDecl){}
    else{
        //多声明（多变量声明、多函数声明、多x声明）
        const DeclGroupRef &dg = declStmt->getDeclGroup();

        //遍历每一个声明
        std::for_each(dg.begin(),dg.end(),[this,likeStruct,typeName,typeClass,&varCnt](const Decl* decl_k){
            bool isStructType_k;
            std::string typeName_k;
            QualType qualType_k;
            this->process_singleDecl(decl_k,isStructType_k,typeName_k,qualType_k);
            clang::Type::TypeClass  typeClass_k = qualType_k->getTypeClass();
            //第k个声明，若是似结构体则记数
            if(isStructType_k){
                varCnt++;
            }
        });
    }

    //是结构体
    if(likeStruct){
        //按照左右花括号，构建位置id，防止重复插入
        LocId declStmtBgnLocId=LocId::buildFor(filePath, declStmtBgnLoc, SM);
        //【执行业务内容】 向threadLocal记录发生一次 :  栈区变量声明 其类型为typeClassName
        //只有似结构体变量才会产生通知
        insertAfter_VarDecl(typeName,varCnt,declStmtBgnLocId,declStmtBgnLoc);
    }

    return result;
}


bool VarVst::process_singleDecl(const Decl *singleDecl, bool& likeStruct, std::string &typeName, QualType &qualType){


    if (const ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(singleDecl)) {
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
            //非结构体
            likeStruct=false;
            std::cout<<"[跳过]isBuiltinType==true;[返回]likeStruct==false\n";
            return true;
        }
        if(isPointerType){
            //非结构体
            likeStruct=false;
            std::cout<<"[跳过]isPointerType==true;[返回]likeStruct==false\n";
            return true;
        }

        //是结构体==是Record或是Elaborated
        likeStruct=(typeClassEqRecord||typeClassEqElaborated);
        MyAssert(likeStruct,"[AssertErr]NotFit:typeClassEqRecord||typeClassEqElaborated");

        std::cout<<fmt::format("[返回]likeStruct=={}\n",likeStruct);

    }

    return true;
}

//TODO 【块离开时， 执行业务内容】 遍历每个k: 直接在该块内声明的栈区变量k，找到变量k的类型名Tk，向threadLocal记录 释放了一个类型为Tk的变量