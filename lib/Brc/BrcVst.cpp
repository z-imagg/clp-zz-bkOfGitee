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

bool BrcVst::letLRBraceWrapRange(SourceLocation B,SourceLocation E, const char* whoInserted ){
  mRewriter_ptr->InsertTextBefore(B,"{");

  std::string comment;
  Util::wrapByComment(whoInserted,comment);
  std::string RBraceStr("}"+comment);

  mRewriter_ptr->InsertTextAfterToken(E,RBraceStr);
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
    std::string comment;
    Util::wrapByComment(whoInserted,comment);
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
  if(thenStmt && !Util::isAloneContainerStmt(thenStmt) )  {
      letLRBraceWrapStmt(thenStmt,"TraverseIfStmt:thenStmt");
  }

  Stmt *elseStmt = ifStmt->getElse();
  if(elseStmt && !Util::isAloneContainerStmt(elseStmt) ) {
      letLRBraceWrapStmt(elseStmt,"TraverseIfStmt:elseStmt");
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
  if(bodyStmt && !Util::isAloneContainerStmt(bodyStmt) )  {
      letLRBraceWrapStmt(bodyStmt,"TraverseWhileStmt");
  }

  //endregion 自定义处理 完毕

  //region  将递归链条正确的接好:  对 当前节点whileStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
//    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
//是否向下层遍历 与 本节点whileStmt的直接子结点 是否为 块语句 无关，因为 whileStmt的深层子结点 可能是块语句
//	即使whileStmt的直接子节点不是块语句 但不影响 whileStmt的深层子结点 可能是块语句
      TraverseStmt(bodyStmt);
//    }
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
  if(bodyStmt && !Util::isAloneContainerStmt(bodyStmt) )  {
      letLRBraceWrapStmt(bodyStmt,"TraverseForStmt");
  }
  //endregion

  //region  将递归链条正确的接好:  对 当前节点forStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
//    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
//是否向下层遍历 与 本节点forStmt的直接子结点 是否为 块语句 无关，因为 forStmt的深层子结点 可能是块语句
//	即使forStmt的直接子节点不是块语句 但不影响 forStmt的深层子结点 可能是块语句
      TraverseStmt(bodyStmt);
//    }
  }
  //endregion
  return true;
}


bool BrcVst::TraverseSwitchStmt(SwitchStmt *switchStmt){
  SwitchCase *caseList = switchStmt->getSwitchCaseList();
  LangOptions &LO = CI.getLangOpts();

  std::vector<SwitchCase*> caseVec;
  //这里假定了: SwitchCase::getNextSwitchCase 获取的顺序 目前 看是  书写case的次序的逆序,
  //    但不能排除 出现  乱须 毫无规律，所以 排序 才靠谱
  for (SwitchCase* switchCase = caseList; switchCase; switchCase = switchCase->getNextSwitchCase()) {
    caseVec.push_back(switchCase);
  }

  //对各个case语句 按开始位置 升序 排序
  std::sort(caseVec.begin(), caseVec.end(), [](clang::SwitchCase* lhs, clang::SwitchCase* rhs) {
      return lhs->getBeginLoc() < rhs->getBeginLoc();
  });

  SourceLocation beginLoc;
  SourceLocation endLoc;
  size_t caseCnt = caseVec.size();
  for(int k=0; k < caseCnt; k++){
    SwitchCase* scK=caseVec[k];


//    beginLoc = scK->getColonLoc();
//    beginLoc = Lexer::getLocForEndOfToken(scK->getColonLoc(), /*Offset*/0, SM, LO);//offset:0 所取得位置是冒号右边 是期望位置
//    beginLoc = Lexer::getLocForEndOfToken(scK->getColonLoc(), /*Offset*/1, SM, LO);//offset:1 所取得位置是冒号左边 非期望位置
//按道理说 offset:1 冒号右边  ，offset:0 冒号左边 才对， 但结果是反过来的。不知道如何解释，据说是因为 冒号这个小Token 所在位置 同时 被一个更大的Token占据，导致offset:1 是 相对于 此大Token的，但这个解释有点勉强。
    //以下2步骤，确定能移动到下一个token位置？
    Token colonTok;
    //步骤1. 取给定位置的Token, 忽略空格、tab等空白符
    Lexer::getRawToken(scK->getColonLoc(), colonTok, SM, LO,/*IgnoreWhiteSpace:*/true);
    //步骤2. 相对 该Token的结束位置 右移1个Token 所获得的位置 即 下一个token位置
    beginLoc = Lexer::getLocForEndOfToken(colonTok.getEndLoc(), /*偏移1个Token*/1, SM, LO);




    if(k<caseCnt-1){
      endLoc=caseVec[k+1]->getBeginLoc();
    }else{
      endLoc=switchStmt->getEndLoc();
    }
    if ( isa<CaseStmt>(*scK)) {
      CaseStmt *caseK = dyn_cast<CaseStmt>(scK);


      //输出case 后表达式
      Expr *LHS = caseK->getLHS();
      LangOptions &LO = CI.getLangOpts();
      Token tk;
      Lexer::getRawToken(LHS->getExprLoc(),tk,SM,LO,true);
      bool invalid;
      const std::string &tkStr = Lexer::getSpelling(tk, SM, LO, &invalid);

      llvm::outs() << "case " << tkStr << ":";

    }else if ( isa<DefaultStmt>(*scK)) {
      DefaultStmt *defaultK = dyn_cast<DefaultStmt>(scK);

      llvm::outs() << "default "  << ":";
    }

    llvm::outs() << "case开始: " << beginLoc.printToString(SM)
    << ", case结束: " << endLoc.printToString(SM) << "\n";
  }

  /**输出
case 1:case开始: /pubx/clang-brc/test_in/test_main.cpp:23:13, case结束: /pubx/clang-brc/test_in/test_main.cpp:27:7
case 2:case开始: /pubx/clang-brc/test_in/test_main.cpp:27:13, case结束: /pubx/clang-brc/test_in/test_main.cpp:31:7
case 3:case开始: /pubx/clang-brc/test_in/test_main.cpp:31:13, case结束: /pubx/clang-brc/test_in/test_main.cpp:33:7
case 4:case开始: /pubx/clang-brc/test_in/test_main.cpp:33:13, case结束: /pubx/clang-brc/test_in/test_main.cpp:40:7
case 6:case开始: /pubx/clang-brc/test_in/test_main.cpp:40:13, case结束: /pubx/clang-brc/test_in/test_main.cpp:44:7
default :case开始: /pubx/clang-brc/test_in/test_main.cpp:44:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:47:5

case语句列表 按 开始位置 升序 排序
当前case结束位置 用下一个case开始位置表示， 最后一个case结束位置 用整个switch的结束位置表示
当前case开始位置 用case或default后的冒号的位置
SwitchCase::getEndLoc 表达的 case结尾位置 基本都不对， case1的结尾只能用case2的开始来表达了。
   */
}


