#include "Var/VarVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>
#include <fmt/core.h>


using namespace llvm;
using namespace clang;



void VarVst::letLRBraceWrapRangeAftBf(SourceLocation B, SourceLocation E, const char* whoInserted ){
}

/**
 * 用左右花括号包裹给定语句
 * @param stmt
 * @param whoInserted
 * @return
 */
void VarVst::letLRBraceWrapStmtBfAfTk(Stmt *stmt, const char* whoInserted){

}



bool VarVst::TraverseIfStmt(IfStmt *ifStmt){


  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return true;
}
bool VarVst::TraverseWhileStmt(WhileStmt *whileStmt){

  //继续遍历剩余源码
//  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
//  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return true;
}

//forEach和for很相似
bool VarVst::TraverseForStmt(ForStmt *forStmt) {

  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return false;
}

//forEach和for很相似
bool VarVst::TraverseCXXForRangeStmt(CXXForRangeStmt *forRangeStmt) {

  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return false;
}

bool VarVst::TraverseSwitchStmt(SwitchStmt *switchStmt){

  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return true;
}



bool VarVst::VisitDeclStmt(DeclStmt* declStmt){



    SourceLocation beginLoc,endLoc;
    beginLoc = declStmt->getBeginLoc();
    endLoc = declStmt->getEndLoc();

    //构造人类可读开始位置、结束位置、插入者 注释文本
    std::string hmTxtCmnt_whoInsrt_BE;
    Util::BE_Loc_HumanText(SM, beginLoc, endLoc, "test_VisitDeclStmt", hmTxtCmnt_whoInsrt_BE);
    std::cout<<hmTxtCmnt_whoInsrt_BE;
    return true;
}