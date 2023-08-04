#include "Brc/BrcVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>
#include <fmt/core.h>


using namespace llvm;
using namespace clang;













bool BrcVst::insertLRBrace(LocId LBraceLocId, SourceLocation LBraceLoc ,LocId RBraceLocId, SourceLocation RBraceLoc , const char* whoInserted){
  return true;
}


/**
 * 用左右花括号包裹给定语句
 * @param stmt
 * @param whoInserted
 * @return
 */
bool BrcVst::letLRBraceWrapStmt(Stmt *stmt, const char* whoInserted){
  mRewriter_ptr->InsertTextBefore(stmt->getBeginLoc(),"{");
  const SourceLocation &stmtEndLoc = stmt->getEndLoc();

  bool endIsSemicolon=false;
  SourceLocation endSemicolonLoc = Util::getStmtEndSemicolonLocation(stmt,SM,endIsSemicolon);
  if(endIsSemicolon){
    const std::string &comment = fmt::format("/*{}*/", whoInserted);
    std::string RBraceStr("}"+comment);
    mRewriter_ptr->InsertTextAfterToken(endSemicolonLoc,RBraceStr);
  }

}



bool BrcVst::TraverseIfStmt(IfStmt *ifStmt){
  //region 若NULL，直接返回
  if(!ifStmt){
    return false;
  }
  //endregion

  //region 自定义处理: if的then语句、else语句 若非块语句 则用花括号包裹

  Stmt *thenStmt = ifStmt->getThen();
  if(thenStmt)  {
    bool thenIsCompoundStmt=isa<CompoundStmt>(*thenStmt);
    if ( !thenIsCompoundStmt ) {
      letLRBraceWrapStmt(thenStmt,"TraverseIfStmt:thenStmt");
    }
  }

  Stmt *elseStmt = ifStmt->getElse();
  if(elseStmt) {
    bool elseIsCompoundStmt=isa<CompoundStmt>(*elseStmt);
    if (!elseIsCompoundStmt ) {
      letLRBraceWrapStmt(elseStmt,"TraverseIfStmt:elseStmt");
    }
  }
//endregion 自定义处理 完毕

  //region  将递归链条正确的接好:  对 当前节点ifStmt的下一层节点child:{then,else}  调用顶层方法TraverseStmt(child)

  if(thenStmt){
      TraverseStmt  (thenStmt);
  }

  if(elseStmt){
    TraverseStmt(elseStmt);
  }
  //endregion

  return true;
}
bool BrcVst::TraverseWhileStmt(WhileStmt *whileStmt){
  //region 若NULL，直接返回
  if(!whileStmt){
    return false;
  }
  //endregion

  //region 自定义处理: while的循环体语句 若非块语句 则用花括号包裹
  Stmt *bodyStmt = whileStmt->getBody();
  if(bodyStmt)  {
    bool bodyIsCompoundStmt=isa<CompoundStmt>(*bodyStmt);
    if ( !bodyIsCompoundStmt ) {
      letLRBraceWrapStmt(bodyStmt,"TraverseWhileStmt");
    }
  }

  //endregion 自定义处理 完毕

  //region  将递归链条正确的接好:  对 当前节点whileStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      TraverseStmt(bodyStmt);
    }
  }
  //endregion

  return true;
}

bool BrcVst::TraverseForStmt(ForStmt *forStmt) {
  //region 若NULL，直接返回
  if(!forStmt){
    return false;
  }
  //endregion

  //region 自定义处理: for的循环体语句 若非块语句 则用花括号包裹
  Stmt *bodyStmt = forStmt->getBody();
  if(bodyStmt)  {
    bool bodyIsCompoundStmt=isa<CompoundStmt>(*bodyStmt);
    if ( !bodyIsCompoundStmt ) {
      letLRBraceWrapStmt(bodyStmt,"TraverseForStmt");
    }
  }
  //endregion

  //region  将递归链条正确的接好:  对 当前节点forStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
//    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      TraverseStmt(bodyStmt);
//    }
  }
  //endregion
  return true;
}



bool BrcVst::TraverseCaseStmt(CaseStmt *caseStmt) {

/////////////////////////对当前节点caseStmt 不做 自定义处理
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点caseStmt的下一层节点中的单情况体  调用顶层方法TraverseStmt(单情况体)
                                /////case的其他子节点，比如 'case 值:' 中的 '值' 不做处理。
  Stmt *body = caseStmt->getSubStmt();//不太确定 case.getSubStmt 是 获取case的单情况体
  if(body){
    Stmt::StmtClass bodyStmtClass = body->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(body);
    }
  }
  return true;
}





