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














void BrcVst::letLRBraceWrapRangeAftBf(SourceLocation B, SourceLocation E, const char* whoInserted ){

  //region 如果被包裹语句 处在宏中 则不处理 直接返回。
  if(
    Util::LocIsInMacro(B,SM)
    ||
    Util::LocIsInMacro(E,SM)
  ){
    return;
  }
  //endregion

  //region 跳过非MainFile. 场景: '#include "xxx.def"', 跳过xxx.def， 即 不修改xxx.def
  if( !Util::LocFileIDEqMainFileID(SM, B) || !Util::LocFileIDEqMainFileID(SM, E) ){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return  ;
  }
  //endregion


  //region 获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);
  //endregion

  //region 若此位置已经插入过左花括号, 则不再插入，防止重复
  LocId LBraceLocId=LocId::buildFor(filePath, B, SM);
  if(Util::LocIdSetContains(LBraceLocIdSet,LBraceLocId)){
    return ;
  }
  //endregion

  //region 插入左右花括号
  mRewriter_ptr->InsertTextAfterToken(B,"{");

  std::string comment;
  Util::wrapByComment(whoInserted,comment);
  std::string RBraceStr("}"+comment);

  //记录已插入左花括号的节点ID们以防重： 即使重复遍历了 但不会重复插入
  LBraceLocIdSet.insert(LBraceLocId);
  
  mRewriter_ptr->InsertTextBefore(E,RBraceStr);
  //endregion
}

/**
 * 用左右花括号包裹给定语句
 * @param stmt
 * @param whoInserted
 * @return
 */
void BrcVst::letLRBraceWrapStmtBfAfTk(Stmt *stmt, const char* whoInserted){
  SourceLocation beginLoc = stmt->getBeginLoc();
  SourceLocation endLoc = stmt->getEndLoc();
  
  //region 如果被包裹语句 处在宏中 则不处理 直接返回。

  if(
    Util::LocIsInMacro(beginLoc,SM)
    ||
    Util::LocIsInMacro(endLoc,SM)
  ){
    return;
  }
  //endregion

  //region 跳过非MainFile. 场景: '#include "xxx.def"', 跳过xxx.def， 即 不修改xxx.def
  if( !Util::LocFileIDEqMainFileID(SM, beginLoc) ){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return  ;
  }
  //endregion


  //region 获取主文件ID,文件路径
  FileID mainFileId;
  std::string filePath;
  Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);
  //endregion

  //region 若此位置已经插入过左花括号, 则不再插入，防止重复
  LocId LBraceLocId=LocId::buildFor(filePath, beginLoc, SM);
  if(Util::LocIdSetContains(LBraceLocIdSet,LBraceLocId)){
    return ;
  }
  //endregion

  //region 插入左右花括号

  bool endIsSemicolon=false;
  SourceLocation endSemicolonLoc = Util::getStmtEndSemicolonLocation(stmt,SM,endIsSemicolon);
  if(endIsSemicolon &&

//bug场景举例:for体为无分号单语句，比如switch，找 该体 末尾分号 将得到for外后其他语句的无关分号 此明显错误,
// 解决办法:找到的分号位置须在体内
//   即 找到的分号位置 必须 小于等于 体结束位置
  endSemicolonLoc<=endLoc
  ){
    //只有找到分号位置，才可以插入左右花括号。
    //   不能造成插入了左花括号，却没找到分号，然后没法插入右花括号，也没法撤销左花括号，而陷入语法错误。
    mRewriter_ptr->InsertTextBefore(stmt->getBeginLoc(),"{");

    std::string comment;
    Util::wrapByComment(whoInserted,comment);
    std::string RBraceStr("}"+comment);

    mRewriter_ptr->InsertTextAfterToken(endSemicolonLoc,RBraceStr);

    //记录已插入左花括号的节点ID们以防重： 即使重复遍历了 但不会重复插入
    LBraceLocIdSet.insert(LBraceLocId);
  }
  //endregion

}



bool BrcVst::TraverseIfStmt(IfStmt *ifStmt){
  //region 若NULL，直接返回
  if(!ifStmt){
    return false;
  }
  //endregion

  //region 跳过非MainFile
  if( !Util::LocFileIDEqMainFileID(SM, ifStmt->getBeginLoc()) ){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return false;
  }
  //endregion
  
  //region 自定义处理: if的then语句、else语句 若非块语句 则用花括号包裹

  Stmt *thenStmt = ifStmt->getThen();
  if(thenStmt && !Util::isAloneContainerStmt(thenStmt) )  {
    letLRBraceWrapStmtBfAfTk(thenStmt, "BrcThen");
  }

  Stmt *elseStmt = ifStmt->getElse();
  if(elseStmt && !Util::isAloneContainerStmt(elseStmt) ) {
    letLRBraceWrapStmtBfAfTk(elseStmt, "BrcElse");
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


  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return true;
}
bool BrcVst::TraverseWhileStmt(WhileStmt *whileStmt){
  //region 若NULL，直接返回
  if(!whileStmt){
    return false;
  }
  //endregion

  //region 跳过非MainFile
  if( !Util::LocFileIDEqMainFileID(SM, whileStmt->getBeginLoc()) ){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return false;
  }
  //endregion

  //region 自定义处理: while的循环体语句 若非块语句 则用花括号包裹
  Stmt *bodyStmt = whileStmt->getBody();
  if(bodyStmt && !Util::isAloneContainerStmt(bodyStmt) )  {
    letLRBraceWrapStmtBfAfTk(bodyStmt, "BrcWhl");
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

  //继续遍历剩余源码
//  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
//  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return true;
}

bool BrcVst::TraverseForStmt(ForStmt *forStmt) {
  //region 若NULL，直接返回
  if(!forStmt){
    return false;
  }
  //endregion

  //region 跳过非MainFile
  if( !Util::LocFileIDEqMainFileID(SM, forStmt->getBeginLoc()) ){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return false;
  }
  //endregion

//  Util::printStmt(*Ctx,CI,"forStmt","",forStmt, true); //开发用打印

  //region 自定义处理: for的循环体语句 若非块语句 则用花括号包裹
  Stmt *bodyStmt = forStmt->getBody();
  if(bodyStmt && !Util::isAloneContainerStmt(bodyStmt) )  {
    letLRBraceWrapStmtBfAfTk(bodyStmt, "BrcFor");
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

  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return false;
}


bool BrcVst::TraverseSwitchStmt(SwitchStmt *switchStmt){
  
  //region 跳过非MainFile
  if( !Util::LocFileIDEqMainFileID(SM, switchStmt->getBeginLoc()) ){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return false;
  }
  //endregion

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
    beginLoc = scK->getColonLoc();




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
      bool caseKInMacro=false;

      //region 该case中是否含有宏，若有宏，不加花括号。 虽然有误杀，但保险。
      std::string msg;
      CaseStmt *caseK=NULL;
      if ( isa<CaseStmt>(*scK)) {
        caseK = dyn_cast<CaseStmt>(scK);
        Stmt::StmtClass caseKSCls = caseK->getStmtClass();//  clang::Stmt::CaseStmtClass

        bool isMacroEllipsisLoc = Util::LocIsInMacro(caseK->getEllipsisLoc(), SM);
        caseKInMacro = caseKInMacro || isMacroEllipsisLoc;
        bool isMacroCaseKBeginLoc = Util::LocIsInMacro(caseK->getBeginLoc(),SM);
        caseKInMacro = caseKInMacro || isMacroCaseKBeginLoc;
        msg=fmt::format("{},isMacroEllipsisLoc={},isMacroCaseKBeginLoc={},",msg,isMacroEllipsisLoc,isMacroCaseKBeginLoc);
      }else if ( isa<DefaultStmt>(*scK)) {
        DefaultStmt *defaultK = dyn_cast<DefaultStmt>(scK);
        bool isMacro_defaultKColonLoc = Util::LocIsInMacro(defaultK->getColonLoc(),SM);
        caseKInMacro = caseKInMacro || isMacro_defaultKColonLoc;
        msg=fmt::format("{},isMacro_defaultKColonLoc={},",msg,isMacro_defaultKColonLoc);
      }


      Stmt *scKSubStmt = scK->getSubStmt();
      bool isMacroScKSubStmtBeginLoc = Util::LocIsInMacro(scKSubStmt->getBeginLoc(),SM);
      caseKInMacro = caseKInMacro || isMacroScKSubStmtBeginLoc;
      bool isMacroScKSubStmtEndLoc = Util::LocIsInMacro(scKSubStmt->getEndLoc(),SM);
      caseKInMacro = caseKInMacro || isMacroScKSubStmtEndLoc;
      msg=fmt::format("{},isMacroScKSubStmtBeginLoc={},isMacroScKSubStmtEndLoc={},",msg,isMacroScKSubStmtBeginLoc,isMacroScKSubStmtEndLoc);

      SourceLocation scKB = scK->getBeginLoc();
      bool isMacro_scKB = Util::LocIsInMacro(scKB,SM);
      caseKInMacro = caseKInMacro || isMacro_scKB;
      msg=fmt::format("{},isMacro_scKB={},",msg,isMacro_scKB);

      SourceLocation scKE = scK->getEndLoc();
      bool isMacro_scKE = Util::LocIsInMacro(scKE,SM);
      caseKInMacro = caseKInMacro || isMacro_scKE;
      msg=fmt::format("{},isMacro_scKE={},",msg,isMacro_scKE);

      bool isMacro_beginLoc = Util::LocIsInMacro(beginLoc,SM);
      caseKInMacro = caseKInMacro || isMacro_beginLoc;
      bool isMacro_endLoc = Util::LocIsInMacro(endLoc,SM);
      caseKInMacro = caseKInMacro || isMacro_endLoc;
      msg=fmt::format("{},isMacro_beginLoc={},isMacro_endLoc={},",msg,isMacro_beginLoc,isMacro_endLoc);

      SourceLocation beginLocNext=Util::nextTokenLocation(beginLoc,SM,LO);
      bool isMacro_beginLocNext = Util::LocIsInMacro(beginLocNext,SM);
      caseKInMacro = caseKInMacro || isMacro_beginLocNext;
      msg=fmt::format("{},isMacro_beginLocNext={},",msg,isMacro_beginLocNext);

      SourceLocation beginLocPrev=Util::nextTokenLocation(beginLoc,SM,LO,-1);
      bool isMacro_beginLocPrev = Util::LocIsInMacro(beginLocPrev,SM);
      caseKInMacro = caseKInMacro || isMacro_beginLocPrev;
      msg=fmt::format("{},isMacro_beginLocPrev={},",msg,isMacro_beginLocPrev);

      SourceLocation endLocPrev=Util::nextTokenLocation(endLoc,SM,LO,-1);
      bool isMacro_endLocPrev = Util::LocIsInMacro(endLocPrev,SM);
      caseKInMacro = caseKInMacro || isMacro_endLocPrev;
      msg=fmt::format("{},isMacro_endLocPrev={},",msg,isMacro_endLocPrev);

      SourceLocation ColonLoc=scK->getColonLoc();
      bool isMacro_ColonLoc = Util::LocIsInMacro(ColonLoc,SM);
      caseKInMacro = caseKInMacro || isMacro_ColonLoc;
      msg=fmt::format("{},isMacro_ColonLoc={},",msg,isMacro_ColonLoc);
      //endregion. 。

      if(
      //该case中若有宏，不加花括号。 虽然有误杀，但保险。
        !caseKInMacro
      ) {
      int line=-1,col=-1;
      Util::extractLineAndColumn(SM,scK->getBeginLoc(),line,col);
      msg=fmt::format("{},line={}...col={},",msg,line,col);
//      Util::printStmt(*Ctx,CI,"scK",msg,scK,true);//开发用
        letLRBraceWrapRangeAftBf(beginLoc, endLoc, "BrcSw");
      }
      

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


  //继续遍历剩余源码
  //  TraverseXxxStmt末尾返回true  表示继续遍历剩余源码
  //  TraverseXxxStmt末尾返回false 表示从此结束遍历，遍历剩余不再遍历
  return true;
}



