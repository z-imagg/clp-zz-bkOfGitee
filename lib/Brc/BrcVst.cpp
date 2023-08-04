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
    mRewriter_ptr->InsertTextAfterToken(endSemicolonLoc,"}");
  }

}



bool BrcVst::TraverseIfStmt(IfStmt *ifStmt){
  /////////////////////////对当前节点compoundStmt做 自定义处理

  if(!ifStmt){
    return false;
  }

  Stmt *thenStmt = ifStmt->getThen();
  if(thenStmt)  {
    bool thenIsCompoundStmt=isa<CompoundStmt>(*thenStmt);
    if ( !thenIsCompoundStmt ) {
      letLRBraceWrapStmt(thenStmt,"");
    }
  }

  Stmt *elseStmt = ifStmt->getElse();
  if(elseStmt) {
    bool elseIsCompoundStmt=isa<CompoundStmt>(*elseStmt);
    if (!elseIsCompoundStmt ) {
      letLRBraceWrapStmt(elseStmt,"");
    }
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点ifStmt的下一层节点child:{then,else}  调用顶层方法TraverseStmt(child)

  if(thenStmt){
      TraverseStmt  (thenStmt);
  }

  if(elseStmt){
    TraverseStmt(elseStmt);
  }

  return true;
}
bool BrcVst::TraverseWhileStmt(WhileStmt *whileStmt){
/////////////////////////对当前节点whileStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,whileStmt)){
    letLRBraceWrapStmt(whileStmt, "TraverseWhileStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点whileStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = whileStmt->getBody();
  if(bodyStmt){

    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      /**只有当while的循环体是 块语句 时, 该 循环体，才需要 经过 TraverseStmt(循环体) ---...--->TraverseCompoundStmt(循环体) 转交，在 TraverseCompoundStmt(循环体) 中 对 该循环体中的每条子语句前 插入 时钟调用语句.
       * 形如:
       * while(...)
       * {
       * ...;//这里是 while的循环体, 是一个块语句，需要 对 循环体中的每条子语句前 插入 时钟调用语句.
       * }
       */
      TraverseStmt(bodyStmt);
    }
    /**否则 while的循环体 肯定是一个单行语句，无需插入 时钟调用语句.
     * 形如 :
     * while(...)
     *   ...;// 这里是 while的循环体, 是一个单行语句，无需插入 时钟调用语句.
     */
  }
  return true;
}

bool BrcVst::TraverseForStmt(ForStmt *forStmt) {
/////////////////////////对当前节点forStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,forStmt)){
    letLRBraceWrapStmt(forStmt, "TraverseForStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点forStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = forStmt->getBody();
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
//    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(bodyStmt);
//    }
  }
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





