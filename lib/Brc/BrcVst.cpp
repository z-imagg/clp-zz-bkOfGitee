#include "Brc/BrcVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "Brc/RangeHasMacroAstVst.h"
#include "Brc/CollectIncMacro_PPCb.h"

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


    //region 开发用输出
//    llvm::outs() << ",是否块:"<< std::to_string(subStmtIsCompound) <<",case开始: " << beginLoc.printToString(SM)
//    << ", case结束: " << endLoc.printToString(SM) << "\n";
    //endregion

    //如果case体不是块，才用花括号包裹.
    if(!subStmtIsCompound){

      //region 用一Visitor遍历该switch中每个语句 且 限制范围为 case内，记录 case内 是否有宏、子语句个数
      //   理论上可以写成 用一Visitor遍历该case中每个语句 且 限制范围为 case内，但这又回到之前的老问题 即 拿不到case的完整子语句们。 但能拿到完整switch啊，所以才有上面遍历办法。
      // case内 即 case冒号后到下一case前内语句，
      //   对比:
      //        Traverse、Visitor 总是能触及到 case内的每条语句的
      //        case.children、case.getSubStmt只能触及到 case内的前部分语句
      SourceRange BE=SourceRange(beginLoc, endLoc);
      RangeHasMacroAstVst rv(CI,BE);
      rv.TraverseStmt(switchStmt);
    //endregion

      //region 如果此case内有宏 或 该case内无语句 或 该case内有#include 或 该case内有#define，则不处理。
      // 如果此case内有宏 或 该case冒号到下'case'前无语句，则不处理。 否则 此case内无宏，则处理
      if(
      //如果此case内有宏，则不处理
      rv.hasMacro ||
      //如果此case内无子语句，则不处理
      rv.caseKSubStmtCnt==0 ||
      //如果此case有 直接写在'case'内的变量声明，则不处理。
      //   理由是，caseK中直接声明的变量可能在caseJ中使用，若caseK被花括号包裹，则caseJ无法使用该变量。
      rv.VarDeclDirectlyInCaseKCnt > 0 ||
      //预处理回调已经收集了#include、#define ，这里判断case起止范围内 有无#i、#d，若有 则不处理该case
      CollectIncMacro_PPCb::hasInclusionDirective(SM, BE) || CollectIncMacro_PPCb::hasMacroDefined(SM, BE)
      ){
        //如果此case内有宏，则不处理
        continue;
      }
      //endregion

      //region 开发用
//      std::string msg;
//      int line=-1,col=-1;
//      Util::extractLineAndColumn(SM,scK->getBeginLoc(),line,col);
//      msg=fmt::format("{},line={}...col={},",msg,line,col);
//      Util::printStmt(*Ctx,CI,"scK",msg,scK,true);//开发用
      //endregion

      //region 否则  处理 此case
      // 否则 此case内 无宏、且有语句，则处理
      letLRBraceWrapRangeAftBf(beginLoc, endLoc, "BrcSw");

      //endregion

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



