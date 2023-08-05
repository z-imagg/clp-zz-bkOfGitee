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














void BrcVst::letLRBraceWrapRangeBfBf(SourceLocation B, SourceLocation E, const char* whoInserted ){
  mRewriter_ptr->InsertTextBefore(B,"{");

  std::string comment;
  Util::wrapByComment(whoInserted,comment);
  std::string RBraceStr("}"+comment);

  mRewriter_ptr->InsertTextBefore(E,RBraceStr);
}

/**
 * 用左右花括号包裹给定语句
 * @param stmt
 * @param whoInserted
 * @return
 */
void BrcVst::letLRBraceWrapStmtBfAfTk(Stmt *stmt, const char* whoInserted){
  mRewriter_ptr->InsertTextBefore(stmt->getBeginLoc(),"{");

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
    letLRBraceWrapStmtBfAfTk(thenStmt, "TraverseIfStmt:thenStmt");
  }

  Stmt *elseStmt = ifStmt->getElse();
  if(elseStmt && !Util::isAloneContainerStmt(elseStmt) ) {
    letLRBraceWrapStmtBfAfTk(elseStmt, "TraverseIfStmt:elseStmt");
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

  return false;
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
    letLRBraceWrapStmtBfAfTk(bodyStmt, "TraverseWhileStmt");
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

  return false;
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
    letLRBraceWrapStmtBfAfTk(bodyStmt, "TraverseForStmt");
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
  return false;
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

    Stmt *subStmt = scK->getSubStmt();
//    Util::printStmt(*Ctx,CI,"sub","",subStmt,true);
//    Util::printStmt(*Ctx,CI,"scK","",scK,true);
    bool subStmtIsCompound = isa<CompoundStmt>(*subStmt);

    //开始位置为冒号的下一个Token所在位置
    //    注意此方法中的代码 是否在任何情况下都能实现 移动到下一个位置 有待确定
    beginLoc = Util::nextTokenLocation(scK->getColonLoc(),SM,LO);




    if(k<caseCnt-1){
      endLoc=caseVec[k+1]->getBeginLoc();
    }else{
      endLoc=switchStmt->getEndLoc();
    }
    if ( isa<CaseStmt>(*scK)) {
      CaseStmt *caseK = dyn_cast<CaseStmt>(scK);


      //region 输出case 后表达式 , 开发用
//      Expr *LHS = caseK->getLHS();
//      LangOptions &LO = CI.getLangOpts();
//      Token tk;
//      Lexer::getRawToken(LHS->getExprLoc(),tk,SM,LO,true);
//      bool invalid;
//      const std::string &tkStr = Lexer::getSpelling(tk, SM, LO, &invalid);
//
//      llvm::outs() << "case " << tkStr << ":";
      //endregion

    }else if ( isa<DefaultStmt>(*scK)) {
      DefaultStmt *defaultK = dyn_cast<DefaultStmt>(scK);

      //region 输出default , 开发用
//      llvm::outs() << "default "  << ":";
      //endregion
    }

//    if(subStmtIsCompound){
//      //case6 应该是块，被 case7 前的注释干扰 导致误判
//      //  TODO： 如果要完善，nextTokenLocation(nextTokenLocation(... 需要N个nextTokenLocation嵌套，其中遇到注释的时候 忽略 继续嵌套, 直到第一个非注释？才做 < endLoc的判断?
//      //  简单点可以把这段 块1后还有语句 的逻辑 整个去掉，会导致 一些需要{}的case 没被加上了{}, 估计影响不大?.
//      bool 块1后还有语句=Util::nextTokenLocation(Util::nextTokenLocation(subStmt->getEndLoc(),SM,LO),SM,LO) < endLoc;
//      if(块1后还有语句){
//        subStmtIsCompound=false;
//      }
//    }

    //region 开发用输出
//    llvm::outs() << ",是否块:"<< std::to_string(subStmtIsCompound) <<",case开始: " << beginLoc.printToString(SM)
//    << ", case结束: " << endLoc.printToString(SM) << "\n";
    //endregion

    //如果case体不是块，才用花括号包裹.
    if(!subStmtIsCompound){
      letLRBraceWrapRangeBfBf(beginLoc, endLoc,
                              "TraverseSwitchStmt"
      );
    }
  }


  /**输出
case 0:,是否块:1,case开始: /pubx/clang-brc/test_in/test_main.cpp:23:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:30:7
case 1:,是否块:0,case开始: /pubx/clang-brc/test_in/test_main.cpp:30:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:34:7
case 2:,是否块:0,case开始: /pubx/clang-brc/test_in/test_main.cpp:34:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:38:7
case 3:,是否块:0,case开始: /pubx/clang-brc/test_in/test_main.cpp:38:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:40:7
case 4:,是否块:1,case开始: /pubx/clang-brc/test_in/test_main.cpp:40:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:47:7
case 6:,是否块:1,case开始: /pubx/clang-brc/test_in/test_main.cpp:47:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:52:7
case 7:,是否块:1,case开始: /pubx/clang-brc/test_in/test_main.cpp:52:14, case结束: /pubx/clang-brc/test_in/test_main.cpp:56:7
default :,是否块:0,case开始: /pubx/clang-brc/test_in/test_main.cpp:56:15, case结束: /pubx/clang-brc/test_in/test_main.cpp:59:5


case语句列表 按 开始位置 升序 排序
当前case结束位置 用下一个case开始位置表示， 最后一个case结束位置 用整个switch的结束位置表示
当前case开始位置 用case或default后的冒号的位置
SwitchCase::getEndLoc 表达的 case结尾位置 基本都不对， case1的结尾只能用case2的开始来表达了。
   */


  //region  将递归链条正确的接好:  对 当前节点ifStmt的下一层节点child:{then,else}  调用顶层方法TraverseStmt(child)
  for(int k=0; k < caseCnt; k++) {
    SwitchCase *scK = caseVec[k];
    TraverseStmt(scK);
  }
  //endregion
  return false;
}


