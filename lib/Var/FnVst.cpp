#include "Var/FnVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "base/ASTContextUtil.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include <vector>

#include <fmt/core.h>
#include <iostream>
#include <clang/AST/ParentMapContext.h>

#include "base/MyAssert.h"
#include "Var/Constant.h"
#include "base/UtilEndStmtOf.h"
#include "base/UtilFuncIsX.h"
#include "base/UtilCompoundStmt.h"
#include "base/UtilLineNum.h"
#include "base/UtilMainFile.h"
#include "base/UtilLocId.h"
#include "base/UtilEnvVar.h"

using namespace llvm;
using namespace clang;


//在函数体左花括号紧后插入'初始化语句'
bool FnVst::insert_init__After_FnBdLBrc( bool useCXX,LocId fnBdLBrcLocId,std::string funcName, SourceLocation funcBodyLBraceLoc , SourceLocation funcBodyRBraceLoc ){

    std::string fnName=Constant::fnNameS__init_varLs[useCXX];
    std::string verbose="";
    //环境变量 clangPlgVerbose_Var 控制 是否在注释中输出完整路径_行号_列号
    if(UtilEnvVar::envVarEq("clangPlgVerbose_Var","true")){
        verbose=fnBdLBrcLocId.to_string();
    }

//在函数左花括号紧后插入  初始化语句'_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("源文件路径","函数名",行号,列号);'

    //region 构造插入语句
    std::string cStr_init=fmt::format(
            "_VarDeclLs * _vdLs={}(\"{}\", \"{}\", {}, {}); /* 初始化函数变量列表, {}*/", // _init_varLs_inFn__RtCxx 或 _init_varLs_inFn__RtC00
            fnName,
            fnBdLBrcLocId.filePath,
            funcName,
            fnBdLBrcLocId.line, fnBdLBrcLocId.column,
            verbose
    );
    llvm::StringRef strRef_init(cStr_init);
    bool insertResult_init=mRewriter_ptr->InsertTextAfterToken(funcBodyLBraceLoc , strRef_init);
    //endregion


    //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    fnBdLBrcLocIdSet.insert(fnBdLBrcLocId);

    return insertResult_init ;
}

bool FnVst::TraverseFunctionDecl(FunctionDecl *funcDecl) {
//  Util::printDecl(*Ctx,CI,"TraverseFunctionDecl","FunDecl源码",funcDecl,true);
    //跳过非MainFile
    bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(SM, funcDecl->getLocation());
    if(!_LocFileIDEqMainFileID){
        return false;
    }
    //跳过 default
//    if(Util::funcIsDefault(funcDecl)){
//        return false;
//    }
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
  UtilCompoundStmt::funcBodyIsCompoundThenGetLRBracLoc(body, compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

    //跳过 函数体内无语句
    int stmtCntInFuncBody= UtilCompoundStmt::childrenCntOfCompoundStmt(compoundStmt);
    if(stmtCntInFuncBody<=0){
        return false;
    }

    //跳过 函数左花括号、右花括号在同一行 且 (todo)函数体内只有一条语句的(难,一个大块复合语句也是一条语句)
    bool funcBodyLRBraceInSameLine=UtilLineNum::isEqSrcLocLineNum(SM,funcBodyLBraceLoc,funcBodyRBraceLoc);
    if(funcBodyLRBraceInSameLine){
        return false;
    }

    //获取最后一条语句
    Stmt *endStmtOfFuncBody = UtilEndStmtOf::endStmtOfCompoundStmt(compoundStmt);

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(SM,mainFileId,filePath);

    //获取函数名称
    const std::string &funcQualifiedName = funcDecl->getQualifiedNameAsString();
//  funcDecl->getNameAsString();
//  funcDecl->getName();

    //按照左右花括号，构建位置id，防止重复插入
    LocId funcBodyLBraceLocId=LocId::buildFor(filePath, funcBodyLBraceLoc, SM);
    LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funcBodyRBraceLoc, SM);

    //获取返回类型
    const QualType funcReturnType = funcDecl->getReturnType();

    bool funcIsStatic = funcDecl->isStatic();
    bool funcIsInline = UtilFuncIsX::funcIsInline(funcDecl);

//    std::string verboseLogMsg=fmt::format("开发查问题日志funcIsStatic_funcIsInline:【{}:{}:{};funcQualifiedName】,funcIsStatic={},funcIsInline={}\n",filePath,funcBodyLBraceLocId.line,funcBodyLBraceLocId.column,funcIsStatic,funcIsInline);
//    std::cout<<verboseLogMsg;

    return this->_Traverse_Func(//其中的insertAfter_X__funcEnter内Vst.fnBdLBrcLocIdSet、funcLocId.locationId相互配合使得funcLocId.locationId作为funcLocId.srcFileId局部下的自增数
            funcReturnType,
            false,
            endStmtOfFuncBody,
            funcBodyLBraceLoc,
            funcBodyRBraceLoc,
            funcBodyLBraceLocId,
            funcBodyRBraceLocId,
            compoundStmt,
            funcQualifiedName

    );
}


bool FnVst::TraverseCXXConstructorDecl(CXXConstructorDecl* cxxCnstrDecl){

    return FnVst::I__TraverseCXXMethodDecl(cxxCnstrDecl, "_CXXConstructorDecl");
}

bool FnVst::TraverseCXXMethodDecl(CXXMethodDecl* cxxMthD){
  //cxxMthD is 'CXXMethodDecl | CXXConstructorDecl | CXXDestructorDecl'

  return FnVst::I__TraverseCXXMethodDecl(cxxMthD, "_CXXMethodDecl");
}

bool FnVst::TraverseCXXConversionDecl(CXXConversionDecl * cxxCnvDecl){
  return FnVst::I__TraverseCXXMethodDecl(cxxCnvDecl,"TraverseCXXConversionDecl");
}

bool FnVst::TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl){
    return FnVst::I__TraverseCXXMethodDecl(cxxDestructorDecl, "_CXXDestructorDecl");
}

bool FnVst::I__TraverseCXXMethodDecl(CXXMethodDecl* cxxMethDecl,const char* who){
  //跳过非MainFile
  bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(SM,cxxMethDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    return false;
  }
  //跳过 default
//  if(Util::funcIsDefault(cxxMethDecl)){
//    return false;
//  }
  //跳过 无函数体
  bool hasBody=cxxMethDecl->hasBody();
  if(!hasBody){
    return false;
  }
  //跳过 constexpr
  bool _isConstexpr = cxxMethDecl->isConstexpr();
  if(_isConstexpr){
    return false;
  }

  //获得 函数体、左右花括号
  Stmt* body = cxxMethDecl->getBody();
  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
  UtilCompoundStmt::funcBodyIsCompoundThenGetLRBracLoc(body, compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

  //跳过 函数体内无语句
  int stmtCntInFuncBody= UtilCompoundStmt::childrenCntOfCompoundStmt(compoundStmt);
  if(stmtCntInFuncBody<=0){
    return false;
  }

  //跳过 函数左花括号、右花括号在同一行 且 (todo)函数体内只有一条语句的(难,一个大块复合语句也是一条语句)
  bool funcBodyLRBraceInSameLine=UtilLineNum::isEqSrcLocLineNum(SM,funcBodyLBraceLoc,funcBodyRBraceLoc);
  if(funcBodyLRBraceInSameLine){
    return false;
  }

  //获取最后一条语句
  Stmt *endStmtOfFuncBody = UtilEndStmtOf::endStmtOfCompoundStmt(compoundStmt);

  //获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(SM,mainFileId,filePath);

  //获取函数名称
  const std::string &funcQualifiedName = cxxMethDecl->getQualifiedNameAsString();

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath,funcBodyLBraceLoc, SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath,funcBodyRBraceLoc, SM);

  //获取返回类型
  const QualType funcReturnType = cxxMethDecl->getReturnType();

  std::string whoReturn=fmt::format("{}:cpp函数尾非return", who);
  return this->_Traverse_Func(
          funcReturnType,
          false,
          endStmtOfFuncBody,
          funcBodyLBraceLoc,
          funcBodyRBraceLoc,
          funcBodyLBraceLocId, funcBodyRBraceLocId,
          compoundStmt,
          funcQualifiedName
      );
}
bool FnVst::TraverseLambdaExpr(LambdaExpr *lambdaExpr) {
  if(sizeof(lambdaExpr)<0){//以这样一句话暂时跳过lambda
    return false;
  }

  //跳过非MainFile
  bool _LocFileIDEqMainFileID=UtilMainFile::LocFileIDEqMainFileID(SM,lambdaExpr->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    return false;
  }
  //跳过 default
  //跳过 无函数体
  //跳过 constexpr

  //如果一个lambda函数, 捕捉了全部外部变量，则不对其进行任何操作。
  // 因为此时lambda内外都有xFuncFrame变量，感觉有点奇怪。
  for (const auto& capture : lambdaExpr->captures()) {
    if (
//        capture.getCaptureKind() != clang::LCK_This ||
        capture.getCaptureKind() == clang::LCK_ByCopy ||
        capture.getCaptureKind() == clang::LCK_ByRef
    ) {
      return false;
    }
  }

  //获得 函数体、左右花括号
//  Stmt* body  = funcDecl->getBody();
  CompoundStmt* compoundStmt = lambdaExpr->getCompoundStmtBody();
//  CompoundStmt* compoundStmt;
  SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
  UtilCompoundStmt::GetCompoundLRBracLoc( compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

  //跳过 函数体内无语句
  int stmtCntInFuncBody= UtilCompoundStmt::childrenCntOfCompoundStmt(compoundStmt);
  if(stmtCntInFuncBody<=0){
    return false;
  }

  //跳过 函数左花括号、右花括号在同一行 且 (todo)函数体内只有一条语句的(难,一个大块复合语句也是一条语句)
  bool funcBodyLRBraceInSameLine=UtilLineNum::isEqSrcLocLineNum(SM,funcBodyLBraceLoc,funcBodyRBraceLoc);
  if(funcBodyLRBraceInSameLine){
    return false;
  }

  //获取最后一条语句
  Stmt *endStmtOfFuncBody = UtilEndStmtOf::endStmtOfCompoundStmt(compoundStmt);

  //获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  UtilMainFile::getMainFileIDMainFilePath(SM,mainFileId,filePath);

  //lambda无函数名称
  const char * funName="";

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath, funcBodyLBraceLoc, SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funcBodyRBraceLoc, SM);

  //获取返回类型
  CXXRecordDecl *cxxRecordDecl = lambdaExpr->getLambdaClass();
  // funcReturnType:
  //        优先lambdaExpr->getLambdaClass()->getLambdaTypeInfo()->getType()，
  //        其次lambdaExpr->getCallOperator()->getReturnType()
  QualType funcReturnType;
  TypeSourceInfo *typeSourceInfo=NULL;
  if(cxxRecordDecl && (typeSourceInfo = cxxRecordDecl->getLambdaTypeInfo()) ){
    funcReturnType=typeSourceInfo->getType();
  }else
  if(CXXMethodDecl *cxxMethodDecl=lambdaExpr->getCallOperator()){
    funcReturnType=cxxMethodDecl->getReturnType();
  }

  //lambda一定有body


  return this->_Traverse_Func(
        funcReturnType,
        false,
        endStmtOfFuncBody,
        funcBodyLBraceLoc,
        funcBodyRBraceLoc,
        funcBodyLBraceLocId,funcBodyRBraceLocId,
        compoundStmt,
        funName
        );
}
bool FnVst::_Traverse_Func(
//        bool funcIsStatic,
//        bool funcIsInline,
        QualType funcReturnType,
        bool isaCXXConstructorDecl,
        Stmt *endStmtOfFuncBody,
        SourceLocation funcBodyLBraceLoc,
        SourceLocation funcBodyRBraceLoc,
        LocId funcBodyLBraceLocId,
        LocId funcBodyRBraceLocId,
        CompoundStmt* compoundStmt,
        std::string funcName
//  bool funcIsConstexpr,
//  bool hasBody,
//  int64_t funcDeclID,
//  Stmt *funcBodyStmt,
)
{
//    Util::printStmt(*Ctx,CI,"_Traverse_Func","查看语句compoundStmt源码",compoundStmt,true);

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理
  bool useCxx = ASTContextUtil::useCxx(Ctx);

    //region 插入 函数进入语句
    if(UtilLocId::LocIdSetNotContains(fnBdLBrcLocIdSet, funcBodyLBraceLocId)){//若没有
//        Util::printStmt(*Ctx, CI, fmt::format("排查问题:{:x},",reinterpret_cast<uintptr_t> (&fnBdLBrcLocIdSet)), funcBodyLBraceLocId.to_csv_line(), compoundStmt, true);

        //若 本函数还 没有 插入 函数进入语句，才插入。
        insert_init__After_FnBdLBrc(useCxx,funcBodyLBraceLocId,funcName, funcBodyLBraceLoc, funcBodyRBraceLoc);
    }
//    }
    //endregion


///////////////////// 自定义处理 完毕

// _Traverse_Func返回true表示继续遍历函数声明
    return true;

}


