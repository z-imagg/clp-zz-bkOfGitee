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

bool VarVst::insertAfterFnLeftBrace__insertBeforeFnRightBrace( LocId funcLocId, SourceLocation funcBodyLBraceLoc , SourceLocation funcBodyRBraceLoc ){

//函数左花括号后 插入 '_VarDeclLs * _vdLs=_init_varLs_inFn("runtime_cpp__vars_fn/test_main__runtime_cpp__vars_fn.cpp","func1",7,14);'

    //region 构造插入语句
    std::string cStr_init=fmt::format(
            "_VarDeclLs * _vdLs=_init_varLs_inFn(\"{}\", \"{}\", {}, {}); /* 初始化函数变量列表, {}*/",
            funcLocId.filePath,
            funcLocId.funcName,
            funcLocId.line, funcLocId.column,
            funcLocId.to_string()
    );
    llvm::StringRef strRef_init(cStr_init);
    bool insertResult_init=mRewriter_ptr->InsertTextAfterToken(funcBodyLBraceLoc , strRef_init);
    //endregion


//【销毁变量通知】  函数右花括号前 插入 'destroyVarLs_inFn(_vdLs);'
    //region 构造插入语句
    std::string cStr_destroy=fmt::format(
            "destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表, {}*/",
            funcLocId.filePath,
            funcLocId.funcName,
            funcLocId.to_string()
    );
    llvm::StringRef strRef_destroy(cStr_destroy);
    bool insertResult_destroy=mRewriter_ptr->InsertTextBefore(funcBodyRBraceLoc , strRef_destroy);
    //endregion


    //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    funcEnterLocIdSet.insert(funcLocId);

    return insertResult_init && insertResult_destroy;
}

bool VarVst::TraverseFunctionDecl(FunctionDecl *funcDecl) {
    //跳过非MainFile
    bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM, funcDecl->getLocation());
    if(!_LocFileIDEqMainFileID){
        return false;
    }
    //跳过 default
    if(Util::funcIsDefault(funcDecl)){
        return false;
    }
    //跳过 无函数体
    bool hasBody=funcDecl->hasBody();
    if(!hasBody){
        return false;
    }
    //跳过 constexpr
    bool _isConstexpr = funcDecl->isConstexpr();
    if(_isConstexpr){
        return false;
    }

    //获得 函数体、左右花括号
    Stmt* body  = funcDecl->getBody();
    CompoundStmt* compoundStmt;
    SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
    Util::funcBodyIsCompoundThenGetLRBracLoc(body, compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

    //跳过 函数体内无语句
    int stmtCntInFuncBody= Util::childrenCntOfCompoundStmt(compoundStmt);
    if(stmtCntInFuncBody<=0){
        return false;
    }

    //获取最后一条语句
    Stmt *endStmtOfFuncBody = Util::endStmtOfCompoundStmt(compoundStmt);

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
    Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);

    //获取函数名称
    const std::string &funcQualifiedName = funcDecl->getQualifiedNameAsString();
//  funcDecl->getNameAsString();
//  funcDecl->getName();

    //按照左右花括号，构建位置id，防止重复插入
    LocId funcBodyLBraceLocId=LocId::buildFor(filePath,funcQualifiedName, funcBodyLBraceLoc, SM);

    //获取返回类型
    const QualType funcReturnType = funcDecl->getReturnType();

    bool funcIsStatic = funcDecl->isStatic();
    bool funcIsInline = Util::funcIsInline(funcDecl);

    std::string verboseLogMsg=fmt::format("开发查问题日志funcIsStatic_funcIsInline:【{}:{}:{};funcQualifiedName】,funcIsStatic={},funcIsInline={}\n",filePath,funcBodyLBraceLocId.line,funcBodyLBraceLocId.column,funcIsStatic,funcIsInline);
    std::cout<<verboseLogMsg;

    return this->_Traverse_Func(//其中的insertAfter_X__funcEnter内Vst.funcEnterLocIdSet、funcLocId.locationId相互配合使得funcLocId.locationId作为funcLocId.srcFileId局部下的自增数
            funcIsStatic,funcIsInline,
            funcReturnType,
            false,
            endStmtOfFuncBody,
            funcBodyLBraceLoc,
            funcBodyRBraceLoc,
            funcBodyLBraceLocId,
            compoundStmt

    );
}


bool VarVst::_Traverse_Func(
        bool funcIsStatic,
        bool funcIsInline,
        QualType funcReturnType,
        bool isaCXXConstructorDecl,
        Stmt *endStmtOfFuncBody,
        SourceLocation funcBodyLBraceLoc,
        SourceLocation funcBodyRBraceLoc,
        LocId funcBodyLBraceLocId,
        CompoundStmt* compoundStmt
//  bool funcIsConstexpr,
//  bool hasBody,
//  int64_t funcDeclID,
//  Stmt *funcBodyStmt,
)
{

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理


    //region 插入 函数进入语句
    if(Util::LocIdSetNotContains(funcEnterLocIdSet, funcBodyLBraceLocId)){//若没有
//        Util::printStmt(*Ctx, CI, fmt::format("排查问题:{:x},",reinterpret_cast<uintptr_t> (&funcEnterLocIdSet)), funcBodyLBraceLocId.to_csv_line(), compoundStmt, true);

        //若 本函数还 没有 插入 函数进入语句，才插入。
        insertAfterFnLeftBrace__insertBeforeFnRightBrace(funcBodyLBraceLocId,funcBodyLBraceLoc,funcBodyRBraceLoc);
    }
//    }
    //endregion


///////////////////// 自定义处理 完毕


    return false;

}

/*
struct _Var
 */
//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
bool VarVst::insertAfter_VarDecl(const std::string typeName,int varCnt,LocId varDeclLocId, SourceLocation varDeclEndLoc ){
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    //region 构造插入语句
    std::string cStr_inserted=fmt::format(
            "createVar(_vdLs, \"{}\", {})  /* 创建变量通知,  {} */ ;",
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
        //  在变量声明语句这，不知道如何获得当前所在函数名 因此暂时函数名传递空字符串
        LocId declStmtBgnLocId=LocId::buildFor(filePath,"", declStmtBgnLoc, SM);
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