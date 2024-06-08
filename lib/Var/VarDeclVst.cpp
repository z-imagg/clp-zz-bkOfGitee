#include "Var/VarDeclVst.h"

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "base/ASTContextUtil.h"
#include "base/StrUtil.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include <vector>

#include <fmt/core.h>
#include <iostream>

#include "base/MyAssert.h"
#include "Var/Constant.h"
#include "base/UtilParentKind.h"
#include "base/UtilMainFile.h"


using namespace llvm;
using namespace clang;


//结构体变量声明末尾 插入 'createVar__RtCxx(_varLs_ptr,"变量类型名",变量个数);'
bool VarDeclVst::insertAfter_VarDecl( bool useCXX,const std::string typeName,int varCnt,LocId varDeclLocId, SourceLocation varDeclEndLoc ){
     std::string fnName=Constant::fnNameS__createVar[useCXX];
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    //region 构造插入语句
    std::string cStr_inserted=fmt::format(
            "{}(_vdLs, \"{}\", {})  /* 创建变量通知,  {} */ ;", // createVar__RtCxx 或 createVar__RtC00
            fnName,typeName, varCnt, varDeclLocId.to_string()
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

bool VarDeclVst::TraverseDeclStmt(DeclStmt* declStmt){

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(SM,mainFileId,filePath);

    const SourceLocation declStmtBgnLoc = declStmt->getEndLoc();

    //忽略for中的循环变量声明, 因该位置插入函数调用语句createVar__RtC**不符合语法
    //忽略'for(T var; var<3; var++) { T var2; }' 中的循环变量声明'T var', 该声明的特征是其父亲是ForStmt
    //   请注意 'T var2;' 的父亲是CompoundStmt
    DynTypedNode parent;
    ASTNodeKind parentNK;
    bool only1P = UtilParentKind::only1ParentNodeKind(CI, *Ctx, declStmt, parent, parentNK);
    assert(only1P);
    bool parentNKIsForStmt = ASTNodeKind::getFromNodeKind<ForStmt>().isSame(parentNK);
    if(parentNKIsForStmt){
        return false;
    }
    //TODO ForEach 要像 ForStmt 一样 处理么?

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

    // 获得 声明 中的 变量类型
    result= this->process_singleDecl(singleDecl, likeStruct, typeName, qualType);
    clang::Type::TypeClass  typeClass = qualType->getTypeClass();

    // 获得 声明 中的 变量个数
    //   单声明 变量个数为1， varCnt已经是1了. 因此单声明这里不做任何处理
    if(isSingleDecl){}
    //   多声明 循环变量们 以获得变量个数
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

  //  Ctx.langOpts.CPlusPlus 估计只能表示当前是用clang++编译的、还是clang编译的, [TODO] 但不能涵盖 'extern c'、'extern c++'造成的语言变化
    bool useCxx=ASTContextUtil::useCxx(Ctx);
    //是结构体
    if(likeStruct){
        //按照左右花括号，构建位置id，防止重复插入
        //  在变量声明语句这，不知道如何获得当前所在函数名 因此暂时函数名传递空字符串
        LocId declStmtBgnLocId=LocId::buildFor(filePath,declStmtBgnLoc, SM);
        //【执行业务内容】 向threadLocal记录发生一次 :  栈区变量声明 其类型为typeClassName
        //只有似结构体变量才会产生通知
        insertAfter_VarDecl(useCxx,typeName,varCnt,declStmtBgnLocId,declStmtBgnLoc);
    }

    return result;
}


bool VarDeclVst::process_singleDecl(const Decl *singleDecl, bool& likeStruct, std::string &typeName, QualType &qualType){


    if (const ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(singleDecl)) {
        qualType = valueDecl->getType();
        clang::Type::TypeClass typeClass = qualType->getTypeClass();
        const clang::Type *typePtr = qualType.getTypePtr();
        const char *typeClassName = typePtr->getTypeClassName();
        bool isBuiltinType = qualType->isBuiltinType();

        bool typeClassEqRecord = clang::Type::Record == typeClass;
        bool typeClassEqElaborated = clang::Type::Elaborated == typeClass;
        bool typeClassEqAuto = clang::Type::Auto == typeClass;

        bool isPointerType=qualType->isPointerType();
        typeName = qualType.getAsString();

//        std::string  msg=fmt::format("typeName='{}',typeClass={},typeClassName={},isBuiltinType={}\n", typeName, (int)typeClass, typeClassName, isBuiltinType);
        //std::cout<<msg;

        if(isBuiltinType){
            //非结构体
            likeStruct=false;
            //std::cout<<"[跳过]isBuiltinType==true;[返回]likeStruct==false\n";
            return true;
        }
        if(isPointerType){
            //非结构体
            likeStruct=false;
            //std::cout<<"[跳过]isPointerType==true;[返回]likeStruct==false\n";
            return true;
        }

        const std::string &qualTypeAsStr = qualType.getAsString();
        //std::cout<<"qualTypeAsStr="<<qualTypeAsStr<<"\n";

        //是lambda表达式
        bool isAuto_Lambda=false;
        //是常规类
        bool isAuto_Regular=false;
        if(typeClassEqAuto){
            //S2是S1前缀，因此S1必须在S2前面，二者顺序不能反
            //S1
            if(StrUtil::startsWith(typeName, "class (lambda at" )){
                isAuto_Lambda=true;
            }
            //S2
            else if(StrUtil::startsWith(typeName, "class " )  ){

                isAuto_Regular= true;
            }
        }

        //是结构体==不是Auto_Lambda且(是Record或是Elaborated或是Auto_常规类)
        likeStruct=( !isAuto_Lambda ) && (typeClassEqRecord||typeClassEqElaborated|| isAuto_Regular);
//        MyAssert(likeStruct,"[AssertErr]NotFit:( !isAuto_Lambda ) && (typeClassEqRecord||typeClassEqElaborated|| isAuto_Regular);");


        //std::cout<<fmt::format("[返回]likeStruct=={}\n",likeStruct);

    }

    return true;
}

/**  clang::Type::Auto == typeClass , 但是typeName不同 , typeName为 'class (lambda at ...)' 或 typeName='class ...'
tag1,title1,parent0NodeKind:CompoundStmt,nodeID:2305,文件路径、坐标:</fridaAnlzAp/clp-zz/test_in/test_main.cpp:31:9, line:36:10>,getStmtClassName:DeclStmt,getStmtClass:12,mainFileId:1,fileId:1,源码:【auto fn_point = [](const Point& point) {
            if(point.x>point.y)
                return point.x+point.y;
            else
                return 0.1;
        }】
typeName='class (lambda at /fridaAnlzAp/clp-zz/test_in/test_main.cpp:31:25)',typeClass=14,typeClassName=Auto,isBuiltinType=false
qualTypeAsStr=class (lambda at /fridaAnlzAp/clp-zz/test_in/test_main.cpp:31:25)
[返回]likeStruct==true

tag1,title1,parent0NodeKind:CompoundStmt,nodeID:2483,文件路径、坐标:</fridaAnlzAp/clp-zz/test_in/test_main.cpp:42:9, col:42>,getStmtClassName:DeclStmt,getStmtClass:12,mainFileId:1,fileId:1,源码:【auto user_auto_var = UserEntity()】
typeName='class UserEntity',typeClass=14,typeClassName=Auto,isBuiltinType=false
qualTypeAsStr=class UserEntity


*/