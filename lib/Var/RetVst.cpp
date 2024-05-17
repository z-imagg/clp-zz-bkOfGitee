#include "Var/RetVst.h"

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
bool RetVst::insert_destroy__Before_fnRet(LocId retBgnLocId, SourceLocation retBgnLoc  ){
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

bool RetVst::TraverseReturnStmt(ReturnStmt *returnStmt){
  //跳过非MainFile
  bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM,returnStmt->getBeginLoc());
  if(!_LocFileIDEqMainFileID){
    return false;
  }

  Util::printStmt(CtxRef,CI,"打印TraverseReturnStmt参数","",returnStmt,true);

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

