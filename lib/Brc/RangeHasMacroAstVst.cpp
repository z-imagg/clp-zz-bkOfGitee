#include "Brc/RangeHasMacroAstVst.h"



bool RangeHasMacroAstVst::VisitStmt(clang::Stmt *stmt) {

  SourceManager &SM = CI.getSourceManager();

  //region 如果遍历到的语句stmt的起至范围 不完全 含在 caseK起至范围内 ，不处理，直接返回。
  // 这种情况可能是拿到了一个更大的非终结符号。
//      bool FC = caseKSrcRange.fullyContains(stmt->getSourceRange());
// 由于 SourceRange::fullyContains 结果是错误的，因此用自制方法Util::fullContains
//      bool FC = CB<=B && CE>=E;//此结果也是错误的， 从 SourceRange::fullyContains 抄来的
  bool FC =  Util::fullContains(SM,caseKSrcRange, stmt->getSourceRange());
//  bool FC_=(caseKBL<=sBL) && (caseKEL>=sEL);//这个结果是对的，完善列号即可。 此即 Util::fullContains
  if(!FC){
    return true;
  }
  //endregion

  //到此处， 语句stmt 一定是 caseK的子语句

  //region caseK的子语句个数自加1
  caseKSubStmtCnt++;
//  Util::printStmt(CI.getASTContext(),CI,"caseK的子语句","",stmt,true);
  //endregion

  SourceLocation B = stmt->getBeginLoc();
  SourceLocation E = stmt->getEndLoc();

  //region 如果已经确定 给定Switch的限定位置范围内 有宏，则直接返回，且不需要再遍历了
  if(hasMacro){
    return false;
  }
  //endregion


  //如果遍历到块语句，不处理，直接返回。因为这里只关心单语句，不关心块语句。
  if (clang::isa<clang::CompoundStmt>(stmt)) {
    return true;
  }





  std::string rvAdrr=fmt::format("{:x}",reinterpret_cast<uintptr_t>(this));
//      Util::printStmt(CI.getASTContext(),CI,"xxx",rvAdrr,stmt,true);


  //如果遍历到的单语句，开始位置在宏中 或 结束位置在宏中，则 给定Switch的限定位置范围内 有宏，直接返回，且不需要再遍历了。
  bool inMacro = Util::LocIsInMacro(B,SM) || Util::LocIsInMacro(E,SM);

  if(!hasMacro ){
    if(inMacro){
      hasMacro=true;
      return false;
    }
  }

  //其他情况，继续遍历
  return true;
}