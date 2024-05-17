#include "Var/FnVst.h"

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

//在return紧前插入'销毁语句'
bool FnVst::insert_destroy__Before_fnRet(LocId retBgnLocId, SourceLocation retBgnLoc  ){
//【销毁变量通知】  函数在return紧前 插入 销毁语句'destroyVarLs_inFn(_vdLs);'
    //region 构造插入语句
    std::string cStr_destroy=fmt::format(
            "destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表, {}*/",
            retBgnLocId.filePath,
            retBgnLocId.funcName,
            retBgnLocId.to_string()
    );
    llvm::StringRef strRef_destroy(cStr_destroy);
    bool insertResult_destroy=mRewriter_ptr->InsertTextBefore(retBgnLoc , strRef_destroy);
    //endregion

  //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  retBgnLocIdSet.insert(retBgnLocId);
  return insertResult_destroy;
}

//在函数体左花括号紧后插入'初始化语句'
bool FnVst::insert_init__After_FnBdLBrc(LocId fnBdLBrcLocId, SourceLocation funcBodyLBraceLoc , SourceLocation funcBodyRBraceLoc ){

//在函数左花括号紧后插入  初始化语句'_VarDeclLs * _vdLs=_init_varLs_inFn("源文件路径","函数名",行号,列号);'

    //region 构造插入语句
    std::string cStr_init=fmt::format(
            "_VarDeclLs * _vdLs=_init_varLs_inFn(\"{}\", \"{}\", {}, {}); /* 初始化函数变量列表, {}*/",
            fnBdLBrcLocId.filePath,
            fnBdLBrcLocId.funcName,
            fnBdLBrcLocId.line, fnBdLBrcLocId.column,
            fnBdLBrcLocId.to_string()
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
    LocId funcBodyRBraceLocId=LocId::buildFor(filePath,funcQualifiedName, funcBodyRBraceLoc, SM);

    //获取返回类型
    const QualType funcReturnType = funcDecl->getReturnType();

    bool funcIsStatic = funcDecl->isStatic();
    bool funcIsInline = Util::funcIsInline(funcDecl);

    std::string verboseLogMsg=fmt::format("开发查问题日志funcIsStatic_funcIsInline:【{}:{}:{};funcQualifiedName】,funcIsStatic={},funcIsInline={}\n",filePath,funcBodyLBraceLocId.line,funcBodyLBraceLocId.column,funcIsStatic,funcIsInline);
    std::cout<<verboseLogMsg;

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
  //跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM, cxxCnstrDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    return false;
  }
  //跳过 default
  if(Util::cxxConstructorIsDefault(cxxCnstrDecl)){
    return true;
  }
  //跳过 无函数体
  bool hasBody=cxxCnstrDecl->hasBody();
  if(!hasBody){
    return false;
  }
  //跳过 constexpr
  bool _isConstexpr = cxxCnstrDecl->isConstexpr();
  if(_isConstexpr){
    return false;
  }

  //获得 函数体、左右花括号
  Stmt* body  = cxxCnstrDecl->getBody();
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
  const std::string &funcQualifiedName = cxxCnstrDecl->getQualifiedNameAsString();

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath,funcQualifiedName, funcBodyLBraceLoc, SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funcQualifiedName,funcBodyRBraceLoc, SM);

  //获取返回类型
  const QualType funcReturnType = cxxCnstrDecl->getReturnType();


  return this->_Traverse_Func(
        funcReturnType,
        true,
        endStmtOfFuncBody,
        funcBodyLBraceLoc,
        funcBodyRBraceLoc,
        funcBodyLBraceLocId,funcBodyRBraceLocId,
        compoundStmt,
        funcQualifiedName
        );
}

bool FnVst::TraverseCXXMethodDecl(CXXMethodDecl* cxxMethDecl){
  return FnVst::I__TraverseCXXMethodDecl(cxxMethDecl,"TraverseCXXMethodDecl");
}

bool FnVst::TraverseCXXConversionDecl(CXXConversionDecl * cxxCnvDecl){
  return FnVst::I__TraverseCXXMethodDecl(cxxCnvDecl,"TraverseCXXConversionDecl");
}
bool FnVst::TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl){
  return FnVst::I__TraverseCXXMethodDecl(cxxDestructorDecl,"CXXDestructorDecl");
}

bool FnVst::I__TraverseCXXMethodDecl(CXXMethodDecl* cxxMethDecl,const char* who){
  //跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,cxxMethDecl->getLocation());
  if(!_LocFileIDEqMainFileID){
    return false;
  }
  //跳过 default
  if(Util::funcIsDefault(cxxMethDecl)){
    return false;
  }
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
  const std::string &funcQualifiedName = cxxMethDecl->getQualifiedNameAsString();

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath, funcQualifiedName,funcBodyLBraceLoc, SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funcQualifiedName,funcBodyRBraceLoc, SM);

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
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,lambdaExpr->getBeginLoc());
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
  Util::GetCompoundLRBracLoc( compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

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

  //lambda无函数名称
  const char * funName="";

  //按照左右花括号，构建位置id，防止重复插入
  LocId funcBodyLBraceLocId=LocId::buildFor(filePath, funName,funcBodyLBraceLoc, SM);
  LocId funcBodyRBraceLocId=LocId::buildFor(filePath, funName,funcBodyRBraceLoc, SM);

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

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理


    //region 插入 函数进入语句
    if(Util::LocIdSetNotContains(fnBdLBrcLocIdSet, funcBodyLBraceLocId)){//若没有
//        Util::printStmt(*Ctx, CI, fmt::format("排查问题:{:x},",reinterpret_cast<uintptr_t> (&fnBdLBrcLocIdSet)), funcBodyLBraceLocId.to_csv_line(), compoundStmt, true);

        //若 本函数还 没有 插入 函数进入语句，才插入。
        insert_init__After_FnBdLBrc(funcBodyLBraceLocId, funcBodyLBraceLoc, funcBodyRBraceLoc);
    }
//    }
    //endregion


///////////////////// 自定义处理 完毕


    return false;

}

bool FnVst::TraverseReturnStmt(ReturnStmt *returnStmt){
  //跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,returnStmt->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    return false;
  }

  //获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);


/////////////////////////对当前节点returnStmt做 自定义处理

//  int64_t returnStmtID = returnStmt->getID(*Ctx);
  const SourceLocation &retBgnLoc = returnStmt->getBeginLoc();
  LocId retBgnLocId=LocId::buildFor(filePath, "", retBgnLoc, SM);
  if(this->retBgnLocIdSet.count(retBgnLocId) > 0){
    //若 已插入  释放栈变量，则不必插入,防止重复。
    return false;
  }

  if(bool parentIsCompound=Util::parentIsCompound(Ctx,returnStmt)){
      insert_destroy__Before_fnRet(retBgnLocId, retBgnLoc);
  }

///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点doStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
//粘接直接子节点到递归链条: TODO: 这段不知道怎么写（得获得return xxx; 的xxx中可能的lambda表达式，并遍历该lambda表达式)， 也有可能不用写：
//希望return true能继续遍历子节点吧，因为return中应该可以写lambda，lambada内有更复杂的函数结构
  return true;
//  Expr *xxx = returnStmt->getRetValue();
}

